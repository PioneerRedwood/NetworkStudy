#include "predef.h"
#include "SimpleSocket.h"



Socket::Socket()
{
	sock = -1;
	clientSock = -1;

	// client, server ��� ���� �ʱ�ȭ
	// https://docs.microsoft.com/en-us/previous-versions/aa454002(v=msdn.10)?redirectedfrom=MSDN
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_port = htons(PORT);
	clientAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
}

Socket::~Socket()
{

}

bool	Socket::Init()
{
#ifdef SERVER
	return InitServer();
#else
	// Client
	return InitClient();
#endif
	return true;
}

void	Socket::UnInit()
{
	CloseSocket();
	recvBufferList.clear();
	sendBufferList.clear();
}

bool	Socket::Connect()
{
	// connect to server
	if (::connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
	{
		// logging here // connect failed error
		return false;
	}
	// FIONBIO ����
	// Function In Out Non-Blocking In Out
#ifdef USE_NONEBLOCK
	unsigned long arg = 1;
	if (ioctlsocket(sock, FIONBIO, &arg) != 0) return false;
#endif
}

void	Socket::CloseSocket()
{
#ifdef SERVER
	CloseServer();
#else
	CloseClient();
#endif // SERVER

}

bool	Socket::InitServer()
{
#ifdef SERVER
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		// Logging here // socket creation error
		return false;
	}

	memset((void*)&server, 0x00, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	if (bind(sock, (struct sockaddr*)&server, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
	{
		// Logging here // bind error
	}

	listen(sock, 5);
	// Logging here // socket created
#endif // SERVER
	return true;

}

bool	Socket::InitClient()
{
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		// Logging here // socket creation error
		return false;
	}

	std::string serverAddress = MemDB::GetInstance()->GetValue("ServerAddress");
	if (serverAddress == "")
	{
		serverAddress = SERVER_ADDRESS;
	}

	// Logging here // server address

	memset((void*)&serverAddr, 0x00, sizeof(serverAddr));

	// inet_addr is deprecated vs 2014
	//serverAddr.sin_addr.S_un.S_addr = inet_addr(serverAddress.c_str());
	serverAddr.sin_family = AF_INET;

	// Utils
	// https://stackoverflow.com/questions/27220/how-to-convert-stdstring-to-lpcwstr-in-c-unicode
	::InetPtonW(AF_INET, Utils::GetInstance()->StringToPCWSTR(serverAddress).c_str(), &serverAddr.sin_addr.S_un.S_addr);
	serverAddr.sin_port = htons(PORT);

	// TCP_NODELAY -> Nagle �˰��� ��� ����: 0(false) Ȥ�� 1(true)
	// ������ ������ ���� ��� ������ �޶�����?
	int flag = 1;
	int ret = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&flag, sizeof(flag));
	if (ret == -1)
	{
		// Logging here // set socket option error
		return false;
	}
	return true;
}

void	Socket::CloseServer()
{
	if (sock > 0)
	{
		::closesocket(sock);
		sock = -1;
	}

	if (clientSock > 0)
	{
		::closesocket(clientSock);
		clientSock = -1;
	}
}

void	Socket::CloseClient()
{
	if (sock == -1) return;
	::closesocket(sock);
	sock = -1;
}

bool	Socket::Update()
{
#ifdef SERVER
	return UpdateServer();
#else
	return UpdateClient();
#endif // SERVER
}

// select socket
bool	Socket::UpdateServer()
{
#ifdef SERVER
	if (clientSock == -1)
	{
		ZeroMemory(&clientAddr, sizeof(struct sockaddr_in));
		int sockLen = sizeof(struct sockaddr_in);

		// ���� �㰡
		if (clientSock = accept(sock, (struct sockaddr*)&clientAddr, &sockLen) == INVALID_SOCKET)
		{
			// Logging here // accept error
			::closesocket(sock);
			return false;
		}

		// client socket -> Non-Block
		unsigned long arg = 1;
		if (ioctlsocket(clientSock, FIONBIO, &arg) != 0) return false;
		// Logging here // new client comes
	}
	else
	{
		// file discriptor set // read fds, write fds
		fd_set read_fds, write_fds;
		struct timeval waitTime;		// the max wait time for an event

		waitTime.tv_sec = 0;
		waitTime.tv_usec = 1000;		// micro seconds
		FD_ZERO(&read_fds);
		FD_ZERO(&write_fds);
		FD_SET(clientSock, &read_fds);

		// ���� �� ������ 
		if (select(clientSock + 1, &read_fds, &write_fds, (fd_set*)0, &waitTime) < 0) return true;

		// ���� �� ������ recv
		if (FD_ISSET(clientSock, &read_fds))
		{
			FD_CLR(clientSock, &read_fds);
			char in[SOCKET_BUFFER];
			memset(&in, 0, sizeof(in));

			int recvSize = recv(clientSock, in, sizeof(in), 0);
			if (recvSize > 0)
			{
				if (recvBuffer.totalSize > 0)
				{
					// �ڿ� �̾� �޾ƾ� ��
					// @�ǹ��� 
					memcpy(recvBuffer.buffer + recvBuffer.totalSize, in, recvSize);
					recvBuffer.totalSize += recvSize;
					RecvDone();
				}
				else
				{
					// ó���� ���
					recvBuffer.totalSize = recvSize;
					memcpy(recvBuffer.buffer, in, recvSize);
					RecvDone();
				}
			}
			else
			{
				// recvSize < 0, If the connection has been gracefully closed, the return value is zero.
				closesocket(clientSock);
				clientSock = -1;
				return true;
			}
		}

		// ���� ���� �ִٸ� ����� �÷��� ����
		if (sendBuffer.totalSize > 0)
		{
			FD_SET(clientSock, &write_fds);
		}

		// send
		if (FD_ISSET(clientSock, &write_fds))
		{
			FD_CLR(clientSock, &write_fds);
			int sendSize = send(clientSock, sendBuffer.buffer + sendBuffer.currentSize, sendBuffer.totalSize - sendBuffer.currentSize, 0);
			if (sendBuffer.totalSize == sendBuffer.currentSize + sendSize)
			{
				SendDone();
			}
			else
			{
				sendBuffer.currentSize += sendSize;
			}
		}
	}
#endif // SERVER
	return true;

}

bool	Socket::UpdateClient()
{
#if 1
	char in[SOCKET_BUFFER];
	memset(&in, 0, sizeof(in));

	int recvSize = recv(sock, in, sizeof(in), 0);
	if (recvSize <= 0)
	{
		// Logging here // socket recv error
		CloseSocket();
		return false;
	}
	else
	{
		if (recvBuffer.totalSize > 0)
		{
			// �ڿ��� �̾� �ޱ�
			// 
			memcpy(recvBuffer.buffer + recvBuffer.totalSize, in, recvSize);
			recvBuffer.totalSize += recvSize;
			RecvDone();
		}
		else
		{
			// ó�� �ޱ�
			recvBuffer.totalSize = recvSize;
			memcpy(recvBuffer.buffer, in, recvSize);

		}
	}
#else
	fd_set read_fds, write_fds;
	struct timeval waitTime;
	int sel;

	waitTime.tv_sec = 0;
	waitTime.tv_usec = 0;
	FD_ZERO(&read_fds);
	FD_ZERO(&write_fds);
	FD_SET(sock, &read_fds);

	sel = select(sock + 1, &read_fds, &write_fds, (fd_set*)0, &waitTime);
	if (sel < 0) return true;

	if (FD_ISSET(sock, &read_fds))
	{
		FD_CLR(sock, &read_fds);
		char in[SOCKET_BUFFER];
		memset(&in, 0, SOCKET_BUFFER);

		int recvSize = recv(sock, in, sizeof(in), 0);
		if (recvSize <= 0)
		{
			// Logging here // socket recv error
			CloseSocket();
			return false;
		}
		else
		{
			if (recvBuffer.totalSize > 0)
			{
				// �ڿ� �̾� �ޱ�
				memcpy(recvBuffer.buffer + recvBuffer.totalSize, in, recvSize);
				recvBuffer.totalSize += recvSize;
				RecvDone();
			}
			else
			{
				// ó�� ����
				recvBuffer.totalSize = recvSize;
				memcpy(recvBuffer.buffer, in, recvSize);
				RecvDone();
			}
		}
	}

	if (sendBuffer.totalSize > 0)
		FD_SET(sock, &write_fds);

	if (FD_ISSET(sock, &write_fds))
	{
		FD_CLR(sock, &write_fds);
		int sendSize = send(sock, sendBuffer.buffer + sendBuffer.currentSize, sendBuffer.totalSize - sendBuffer.currentSize, 0);
		if (sendBuffer.totalSize == sendBuffer.currentSize + sendSize)
		{
			SendDone();
		}
		else
		{
			sendBuffer.currentSize += sendSize;
		}
	}
#endif // 1
	return true;
}

bool	Socket::SendPacket(char* packet, int packetSize)
{
	if (sendBuffer.totalSize > 0)
	{
		// ���� ���̶� �׾� ���´�
		SocketBuffer buf;
		buf.totalSize = packetSize;
		memcpy(buf.buffer, packet, packetSize);
		sendBufferList.push_back(buf);
	}
	else
	{
		// �ٷ� �����Ұŷ� �̵�
		sendBuffer.totalSize = packetSize;
		sendBuffer.currentSize = 0;
		memcpy(sendBuffer.buffer, packet, packetSize);
	}
	return true;
}

bool	Socket::RecvPacket(SocketBuffer* buffer)
{
	if (!recvBufferList.empty())
	{
		buffer->totalSize = recvBufferList[0].totalSize;
		buffer->currentSize = recvBufferList[0].currentSize;
		memcpy(buffer->buffer, recvBufferList[0].buffer, SOCKET_BUFFER);
		recvBufferList.pop_front();
		return true;
	}
}

int Socket::SendImmediate(char* buffer, int dataSize)
{
	return send(sock, buffer, dataSize, 0);
}

void	Socket::SendDone()
{
	// ���� ���� ����Ʈ�� ������� ������
	if (!sendBufferList.empty())
	{
		// ���� ���ۿ� ����Ʈ ù��° ���� ����
		sendBuffer.totalSize = sendBufferList[0].totalSize;
		sendBuffer.currentSize = 0;
		memcpy(sendBuffer.buffer, sendBufferList[0].buffer, sendBufferList[0].totalSize);
		sendBufferList.pop_front();
	}
	else
	{
		// ��������� ���� ���۸� �ʱ�ȭ
		sendBuffer.totalSize = -1;
		sendBuffer.currentSize = 0;
		memset(sendBuffer.buffer, 0, SOCKET_BUFFER);
	}
}

void	Socket::RecvDone()
{
	while (1)
	{
		if (recvBuffer.totalSize >= sizeof(int) + sizeof(char))
		{
			int dataSize = (int&)*recvBuffer.buffer;
			if (recvBuffer.totalSize >= sizeof(int) + sizeof(char) + dataSize)
			{
				SocketBuffer buffer;
				buffer.totalSize = sizeof(int) + sizeof(char) + dataSize;
				memcpy(buffer.buffer, recvBuffer.buffer, buffer.totalSize);

				recvBuffer.totalSize -= buffer.totalSize;

				if (recvBuffer.totalSize > 0)
				{
					char tempBuffer[SOCKET_BUFFER] = { 0, };
					memcpy(tempBuffer, recvBuffer.buffer + buffer.totalSize, recvBuffer.totalSize);
					memcpy(recvBuffer.buffer, tempBuffer, SOCKET_BUFFER);
				}
			}
			else
				break;
		}
		else
			break;
	}
}