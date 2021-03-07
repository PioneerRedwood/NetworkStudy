#include "SimpleSocket.h"

SimpleSocket::SimpleSocket()
{
	WSAData data;
	if(WSAStartup(MAKEWORD(2,2), &data) != 0)
	{
		Logger::Log(WSAGetLastError(), "WSAStartup error\n");
		return;
	}

	Logger::Log(LOG_INFO, "Socket Init gracefully\n");
	serverSock = -1;
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

SimpleSocket::~SimpleSocket()
{
	WSACleanup();
}

bool	SimpleSocket::Init()
{
#ifdef SERVER
	return InitServer();
#else
	// Client
	return InitClient();
#endif
	return true;
}

bool	SimpleSocket::InitServer()
{
#ifdef SERVER
	serverSock = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSock == -1)
	{
		Logger::Log(LOG_ERROR, "Could not create socket\n");
		return false;
	}

	memset((void*)&serverAddr, 0x00, sizeof(serverAddr));

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(PORT);

	if (bind(serverSock, (struct sockaddr*)&serverAddr, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
	{
		Logger::Log(LOG_ERROR, "Bind error\n");
	}

	listen(serverSock, 5);
	Logger::Log(LOG_INFO, "Socket created gracefully\n");
#endif // SERVER
	return true;
}

bool	SimpleSocket::InitClient()
{
	clientSock = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSock == -1)
	{
		Logger::Log(LOG_INFO, "Socket Created\n");
		return false;
	}

	std::string serverAddress = MemDB::GetInstance()->GetValue("ServerAddress");
	if (serverAddress == "")
	{
		serverAddress = SERVER_ADDRESS;
	}

	memset((void*)&serverAddr, 0x00, sizeof(serverAddr));

	// inet_addr is deprecated vs 2014
	//serverAddr.sin_addr.S_un.S_addr = inet_addr(serverAddress.c_str());
	serverAddr.sin_family = AF_INET;

	// std::wstring		Utils::StringToPCWSTR(const std::string& str)
	// https://stackoverflow.com/questions/27220/how-to-convert-stdstring-to-lpcwstr-in-c-unicode
	::InetPtonW(AF_INET, Utils::StringToPCWSTR(serverAddress).c_str(), &serverAddr.sin_addr.S_un.S_addr);
	serverAddr.sin_port = htons(PORT);

	// TCP_NODELAY -> Nagle �˰��� ��� ����: 0(false) Ȥ�� 1(true)
	// ������ ������ ���� ��� ������ �޶�����?
	int flag = 1;
	int ret = setsockopt(clientSock, IPPROTO_TCP, TCP_NODELAY, (const char*)&flag, sizeof(flag));
	if (ret == -1)
	{
		Logger::Log(LOG_ERROR, "Set socket option fail\n");
		return false;
	}
	return true;
}

void	SimpleSocket::UnInit()
{
	CloseSocket();
	recvBufferList.clear();
	sendBufferList.clear();
}

bool	SimpleSocket::Connect()
{
#ifdef SERVER
	return false;
#else
	// connect to server
	if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
	{
		Logger::Log(LOG_ERROR, "Connect fail\n");
		return false;
	}
	// FIONBIO ����
	// Function In Out Non-Blocking In Out
//#ifdef USE_NONEBLOCK
//	unsigned long arg = 1;
//	if (ioctlsocket(sock, FIONBIO, &arg) != 0) return false;
//#endif
	unsigned long arg = 1;
	if (ioctlsocket(sock, FIONBIO, &arg) != 0)
	{
		Logger::Log(LOG_ERROR, "Set Non-blocking fail\n");
		return false;
	}

	return true;
#endif

}

void	SimpleSocket::CloseSocket()
{
#ifdef SERVER
	CloseServer();
#else
	CloseClient();
#endif // SERVER
}

void	SimpleSocket::CloseServer()
{
	if (serverSock > 0)
	{
		::closesocket(serverSock);
		serverSock = -1;
	}

	if (clientSock > 0)
	{
		::closesocket(clientSock);
		clientSock = -1;
	}
}

void	SimpleSocket::CloseClient()
{
	if (clientSock == -1)
		return;
	::closesocket(clientSock);
	clientSock = -1;
}

bool	SimpleSocket::Update()
{
#ifdef SERVER
	return UpdateServer();
#else
	return UpdateClient();
#endif // SERVER
}

bool	SimpleSocket::UpdateServer()
{
#ifdef SERVER
	// ó�� �� ��
	if (clientSock == -1)
	{
		ZeroMemory(&clientAddr, sizeof(struct sockaddr_in));
		int sockLen = sizeof(struct sockaddr_in);

		// ���� �㰡
		clientSock = accept(serverSock, (struct sockaddr*)&clientAddr, &sockLen);
		if (clientSock == INVALID_SOCKET)
		{
			Logger::Log(LOG_INFO, "Accept fail\n");
			::closesocket(serverSock);
			return false;
		}

		// sock -> Non-Block
		unsigned long arg = 1;
		int err = ioctlsocket(serverSock, FIONBIO, &arg);
		if (err == SOCKET_ERROR)
		{
			Logger::Log(LOG_ERROR, "set non-blocking fail.\n");
			return false;
		}

		// Multicast in here
		const char* greeting = "Welcome!!";
		err = send(clientSock, greeting, sizeof(greeting), 0);
		if (err == SOCKET_ERROR)
		{
			Logger::Log(LOG_ERROR, "send greeting fail.%d\n", WSAGetLastError);
			return false;
		}

		Logger::Log(LOG_INFO, "New client connected. %d\n", clientSock);
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
		if (select((int)clientSock + 1, &read_fds, &write_fds, (fd_set*)0, &waitTime) < 0)
		{
			return true;
		}

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

bool	SimpleSocket::UpdateClient()
{
#if 1
	char in[SOCKET_BUFFER];
	memset(&in, 0, sizeof(in));

	int recvSize = recv(clientSock, in, sizeof(in), 0);
	if (recvSize <= 0)
	{
		Logger::Log(LOG_ERROR, "Socket recv fail\n");
		CloseSocket();
		return false;
	}
	else
	{
		if (recvBuffer.totalSize > 0)
		{
			// �ڿ��� �̾� �ޱ�
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

	sel = select(clientSock + 1, &read_fds, &write_fds, (fd_set*)0, &waitTime);
	if (sel < 0) return true;

	if (FD_ISSET(clientSock, &read_fds))
	{
		FD_CLR(clientSock, &read_fds);
		char in[SOCKET_BUFFER];
		memset(&in, 0, SOCKET_BUFFER);

		int recvSize = recv(clientSock, in, sizeof(in), 0);
		if (recvSize <= 0)
		{
			Logger::Log(LOG_ERROR, "Socket recv fail\n");
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

bool	SimpleSocket::SendPacket(const char* packet, int packetSize)
{
	if (sendBuffer.totalSize > 0)
	{
		// ���� ���̶� �׾� ���´�
		SocketBuffer buf;
		buf.totalSize = packetSize;
		memcpy(buf.buffer, packet, packetSize);
		sendBufferList.push_back(buf);
		std::cout << "sendBufferList size: " << sendBufferList.size() << std::endl;
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

bool	SimpleSocket::RecvPacket(SocketBuffer* buffer)
{
	// ���� ���� ����Ʈ�� ������� ������
	if (!recvBufferList.empty())
	{
		// ���� ���� ����Ʈ ù��° ���ڸ� �Ű������� ���޵� ���ۿ� ����
		buffer->totalSize = recvBufferList[0].totalSize;
		buffer->currentSize = recvBufferList[0].currentSize;
		memcpy(buffer->buffer, recvBufferList[0].buffer, SOCKET_BUFFER);
		// ���� ���� ����Ʈ���� ������ ����
		recvBufferList.pop_front();
		return true;
	}
	return false;
}

int		SimpleSocket::SendImmediate(const char* buffer, int dataSize)
{
	return send(serverSock, buffer, dataSize, 0);
}

void	SimpleSocket::SendDone()
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

void	SimpleSocket::RecvDone()
{
	while (1)
	{
		// int + char ũ�� �̻� ���� ���ۿ� ������ �ִٸ�
		if (recvBuffer.totalSize >= sizeof(int) + sizeof(char))
		{
			// �̰� ���� �ǹ�?
			int dataSize = (int&)*recvBuffer.buffer;
			if (recvBuffer.totalSize >= sizeof(int) + sizeof(char) + dataSize)
			{
				SocketBuffer buffer;
				buffer.totalSize = sizeof(int) + sizeof(char) + dataSize;
				memcpy(buffer.buffer, recvBuffer.buffer, buffer.totalSize);
				recvBufferList.push_back(buffer);

				recvBuffer.totalSize -= buffer.totalSize;

				// ���� �����ִ°�?
				if (recvBuffer.totalSize > 0)
				{
					char tempBuffer[SOCKET_BUFFER] = { 0, };
					memcpy(tempBuffer, recvBuffer.buffer + buffer.totalSize, recvBuffer.totalSize);

					// ������ �־�� �����ϴ� ����� ����� �����ؾ� �ҵ�
					//memcpy(recvBuffer.buffer, tempBuffer, SOCKET_BUFFER);
				}
			}
			else
				break;
		}
		else
			break;
	}
}