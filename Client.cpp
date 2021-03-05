#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>

#include <iostream>
#include <vector>
#include <string>
#include <deque>

#define CLIENT 1
#define SERVERADDRESS "127.0.0.1"
#define PORT 8888
#define SOCKET_BUFFER 2048

#pragma comment(lib, "ws2_32.lib")

struct SocketBuffer
{
	char buffer[SOCKET_BUFFER];
	int totalSize;
	int currentSize;

	SocketBuffer()
	{
		memset(buffer, 0x00, SOCKET_BUFFER);
		totalSize = -1;
		currentSize = 0;
	}

};
SocketBuffer* recvBuffer = new SocketBuffer();

static std::wstring StringToPCWSTR(const std::string& str)
{
	int len;
	int strLength = (int)str.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), strLength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), strLength, buf, len);
	std::wstring r(buf);
	delete[] buf;

	return r;
}

void CloseClient(int errNo, std::string errMsg, SOCKET socket)
{
	std::cout << errNo << " " << errMsg << std::endl;
	WSACleanup();
	closesocket(socket);
	exit(0);
}

int _main()
{
	WSAData _data;
	sockaddr_in serverAddr;

	int _logNo = WSAStartup(MAKEWORD(2, 2), &_data);
	if (_logNo != 0)
	{
		std::cout << WSAGetLastError() << " WSAStartup fail\n";
		return 0;
	}

	SOCKET _socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket == INVALID_SOCKET)
	{
		CloseClient((int)WSAGetLastError, " create socket fail.", _socket);
	}

	memset((void*)&serverAddr, 0x00, sizeof(serverAddr));

	serverAddr.sin_family = AF_INET;
	InetPtonW(AF_INET, StringToPCWSTR(SERVERADDRESS).c_str(), &serverAddr.sin_addr.S_un.S_addr);
	serverAddr.sin_port = htons(PORT);

	int flag = 1;
	_logNo = setsockopt(_socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&flag, sizeof(flag));
	if (_logNo)
	{
		CloseClient((int)WSAGetLastError, " set socket option fail.", _socket);
	}

	_logNo = connect(_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (_logNo < 0)
	{
		CloseClient((int)WSAGetLastError, " connect fail.", _socket);
	}

	// set non-block
	unsigned long arg = 1;
	if (ioctlsocket(_socket, FIONBIO, &arg) != 0)
	{
		CloseClient((int)WSAGetLastError, " set socket Non-blocking fail.", _socket);
	}

	// 송/수신 버퍼
	SocketBuffer* recvBuffer = new SocketBuffer();
	SocketBuffer* sendBuffer = new SocketBuffer();

	// 읽기/쓰기 파일 디스크립터
	fd_set read_fds, write_fds;
	struct timeval waitTime;
	int sel;

	waitTime.tv_sec = 1;
	waitTime.tv_usec = 0;
	FD_ZERO(&read_fds);
	FD_ZERO(&write_fds);
	FD_SET(_socket, &read_fds);

	char line[256] = { 0, };
	while (true)
	{
		std::cout << "> ";
		std::cin.getline(line, sizeof(line));

		if (strcmp(line, "Quit") == 0)
		{
			break;
		}
		else
		{
			while (true)
			{
				sel = select(_socket + 1, &read_fds, &write_fds, (fd_set*)0, &waitTime);
				if (sel < 0)
				{
					break;
				}

				// 읽을게 있다면
				if (FD_ISSET(_socket, &read_fds))
				{
					FD_CLR(_socket, &read_fds);
					char in[SOCKET_BUFFER];
					memset(&in, 0, SOCKET_BUFFER);

					int recvSize = recv(_socket, in, sizeof(in), 0);
					if (recvSize <= 0)
					{
						return false;
					}
					else
					{
						if (recvBuffer->totalSize > 0)
						{
							// 뒤에 이어 받기
							memcpy(recvBuffer->buffer + recvBuffer->totalSize, in, recvSize);
							recvBuffer->totalSize += recvSize;
							//RecvDone();
						}
						else
						{
							// 처음 받음
							recvBuffer->totalSize = recvSize;
							memcpy(recvBuffer->buffer, in, recvSize);
							std::cout << in << std::endl;
							//RecvDone();
						}

						if (sendBuffer->totalSize > 0)
						{
							FD_SET(_socket, &write_fds);
						}

						if (FD_ISSET(_socket, &write_fds))
						{
							FD_CLR(_socket, &write_fds);
							int sendSize = send(_socket, sendBuffer->buffer + sendBuffer->currentSize, sendBuffer->totalSize - sendBuffer->currentSize, 0);
							if (sendBuffer->totalSize == sendBuffer->currentSize + sendSize)
							{
								//SendDone();
							}
							else
							{
								sendBuffer->currentSize += sendSize;
							}
						}
					}
				}
			}
		}
	}

	CloseClient(0, "Exit", _socket);
	return 0;
}