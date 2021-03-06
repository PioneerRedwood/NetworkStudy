//#include <WinSock2.h>
//#include <Windows.h>
//#include <WS2tcpip.h>
//
//#include <iostream>
//#include <vector>
//#include <string>
//#include <deque>
//
//#define CLIENT 1
//#define SERVERADDRESS "127.0.0.1"
//#define PORT 8888
//#define SOCKET_BUFFER 2048
//
//#pragma comment(lib, "ws2_32.lib")
//
//struct SocketBuffer
//{
//	char buffer[SOCKET_BUFFER];
//	int totalSize;
//	int currentSize;
//
//	SocketBuffer()
//	{
//		memset(buffer, 0x00, SOCKET_BUFFER);
//		totalSize = -1;
//		currentSize = 0;
//	}
//
//};
//SocketBuffer* recvBuffer = new SocketBuffer();
//
//static std::wstring StringToPCWSTR(const std::string& str)
//{
//	int len;
//	int strLength = (int)str.length() + 1;
//	len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), strLength, 0, 0);
//	wchar_t* buf = new wchar_t[len];
//	MultiByteToWideChar(CP_ACP, 0, str.c_str(), strLength, buf, len);
//	std::wstring r(buf);
//	delete[] buf;
//
//	return r;
//}
//
//void CloseClient(int errNo, std::string errMsg, SOCKET socket)
//{
//	std::cout << errNo << " " << errMsg << std::endl;
//	WSACleanup();
//	closesocket(socket);
//	exit(0);
//}
//
//int _main()
//{
//	WSAData _data;
//	sockaddr_in serverAddr;
//
//	int _logNo = WSAStartup(MAKEWORD(2, 2), &_data);
//	if (_logNo != 0)
//	{
//		std::cout << WSAGetLastError() << " WSAStartup fail\n";
//		return 0;
//	}
//
//	SOCKET _socket = socket(AF_INET, SOCK_STREAM, 0);
//	if (_socket == INVALID_SOCKET)
//	{
//		CloseClient((int)WSAGetLastError, " create socket fail.", _socket);
//	}
//
//	memset((void*)&serverAddr, 0x00, sizeof(serverAddr));
//
//	serverAddr.sin_family = AF_INET;
//	InetPtonW(AF_INET, StringToPCWSTR(SERVERADDRESS).c_str(), &serverAddr.sin_addr.S_un.S_addr);
//	serverAddr.sin_port = htons(PORT);
//
//	int flag = 1;
//	_logNo = setsockopt(_socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&flag, sizeof(flag));
//	if (_logNo)
//	{
//		CloseClient((int)WSAGetLastError, " set socket option fail.", _socket);
//	}
//
//	_logNo = connect(_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
//	if (_logNo < 0)
//	{
//		CloseClient((int)WSAGetLastError, " connect fail.", _socket);
//	}
//
//	// set non-block
//	unsigned long arg = 1;
//	if (ioctlsocket(_socket, FIONBIO, &arg) != 0)
//	{
//		CloseClient((int)WSAGetLastError, " set socket Non-blocking fail.", _socket);
//	}
//
//	// 송/수신 버퍼
//	SocketBuffer* recvBuffer = new SocketBuffer();
//	SocketBuffer* sendBuffer = new SocketBuffer();
//
//	// 읽기/쓰기 파일 디스크립터
//	fd_set read_fds, write_fds;
//	struct timeval waitTime;
//	int sel;
//
//	waitTime.tv_sec = 1;
//	waitTime.tv_usec = 0;
//	FD_ZERO(&read_fds);
//	FD_ZERO(&write_fds);
//	FD_SET(_socket, &read_fds);
//
//	char line[256] = { 0, };
//	while (true)
//	{
//		std::cout << "> ";
//		std::cin.getline(line, sizeof(line));
//
//		if (strcmp(line, "Quit") == 0)
//		{
//			break;
//		}
//		else
//		{
//			while (true)
//			{
//				sel = select(_socket + 1, &read_fds, &write_fds, (fd_set*)0, &waitTime);
//				if (sel < 0)
//				{
//					break;
//				}
//
//				// 읽을게 있다면
//				if (FD_ISSET(_socket, &read_fds))
//				{
//					FD_CLR(_socket, &read_fds);
//					char in[SOCKET_BUFFER];
//					memset(&in, 0, SOCKET_BUFFER);
//
//					int recvSize = recv(_socket, in, sizeof(in), 0);
//					if (recvSize <= 0)
//					{
//						return false;
//					}
//					else
//					{
//						if (recvBuffer->totalSize > 0)
//						{
//							// 뒤에 이어 받기
//							memcpy(recvBuffer->buffer + recvBuffer->totalSize, in, recvSize);
//							recvBuffer->totalSize += recvSize;
//							//RecvDone();
//						}
//						else
//						{
//							// 처음 받음
//							recvBuffer->totalSize = recvSize;
//							memcpy(recvBuffer->buffer, in, recvSize);
//							std::cout << in << std::endl;
//							//RecvDone();
//						}
//
//						if (sendBuffer->totalSize > 0)
//						{
//							FD_SET(_socket, &write_fds);
//						}
//
//						if (FD_ISSET(_socket, &write_fds))
//						{
//							FD_CLR(_socket, &write_fds);
//							int sendSize = send(_socket, sendBuffer->buffer + sendBuffer->currentSize, sendBuffer->totalSize - sendBuffer->currentSize, 0);
//							if (sendBuffer->totalSize == sendBuffer->currentSize + sendSize)
//							{
//								//SendDone();
//							}
//							else
//							{
//								sendBuffer->currentSize += sendSize;
//							}
//						}
//					}
//				}
//			}
//		}
//	}
//
//	CloseClient(0, "Exit", _socket);
//	return 0;
//}

// Windows socket 프로그래밍 클라이언트 코드 <다중 연결>
#define WIN32_LEAN_AND_MEAN

#include	<iostream>
#include	<string>
#include	<ws2tcpip.h>
#include	<winsock2.h>
#include	<thread>
#include	<deque>
#include	<mutex>
#include	<memory>

#include	"Utils.h"

using namespace std;

#pragma comment (lib, "Ws2_32.lib")

#define BUFFER_SIZE		2048
#define LOCALHOST		"127.0.0.1"
#define PORT			"9000"

////////////////////////////////////////////////////

#define LOG_CONNECTION			"Connected"
#define MAX_CONNECTION_DELAY	5

//Logger로 바꿀 것

void shoutError(string errormsg)
{
	cout << errormsg << endl;
	WSACleanup();
	exit(1);
}

void shoutError(addrinfo* res, SOCKET sock, string errormsg)
{
	cout << errormsg << endl;
	freeaddrinfo(res);
	closesocket(sock);
	WSACleanup();
	exit(1);
}

// Logger로 바꿀 것

struct SocketBuffer
{
	char buffer[BUFFER_SIZE];

	SocketBuffer()
	{
		ZeroMemory(buffer, BUFFER_SIZE);
	}

	SocketBuffer(const char* buf)
	{
		memcpy(buffer, buf, BUFFER_SIZE);
	}
};

void SendDone(std::deque<SocketBuffer> sendPipe, SocketBuffer* sendBuffer)
{
	if (!sendPipe.empty())
	{
		memcpy(sendBuffer, &sendPipe.back(), sizeof(SocketBuffer));
		sendPipe.pop_back();
	}
	else
	{
		ZeroMemory(sendBuffer->buffer, sizeof(sendBuffer->buffer));
	}
}

int main()
{
	WSADATA wsaData;
	int iResult;
	char buffer[BUFFER_SIZE];

	struct addrinfo* result = NULL, hints;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult == INVALID_SOCKET)
		shoutError("WSAStartup() failed!\n");

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// 서버 주소와 포트를 설정
	iResult = getaddrinfo(LOCALHOST, PORT, &hints, &result);
	if (iResult != 0)
		shoutError("getaddrinfo() failed!\n");

	SOCKET ConnectSocket = INVALID_SOCKET;

	ConnectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ConnectSocket == INVALID_SOCKET)
		shoutError(result, ConnectSocket, "socket error!\n");

	// 서버에 연결
	iResult = connect(ConnectSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
		shoutError(result, ConnectSocket, "connect() error!\n");

	std::deque<SocketBuffer> sendPipe;
	SocketBuffer* sendBuffer = new SocketBuffer();

	fd_set masterfds, readfds;
	struct timeval waitTime;
	int sel;

	waitTime.tv_sec = 2;
	waitTime.tv_usec = 0;

	FD_ZERO(&masterfds);
	FD_ZERO(&readfds);

	FD_SET(ConnectSocket, &masterfds);

	while (true)
	{
		readfds = masterfds;

		sel = select(ConnectSocket, &readfds, nullptr, nullptr, &waitTime);

		if (sel >= 0)
		{
			// fds 탐색
			if (FD_ISSET(ConnectSocket, &readfds))
			{
				ZeroMemory(buffer, BUFFER_SIZE);

				iResult = recv(ConnectSocket, buffer, BUFFER_SIZE, 0);
				if (iResult > 0)
				{
					std::cout << "recv: " << buffer << std::endl;
					// ECHO
					//sendBuffer = new SocketBuffer(buffer);
					//sendPipe.push_front(*sendBuffer);
				}
			}

			std::string str = Utils::GetCurrentDateTime();
			const char* now = str.c_str();

			ZeroMemory(buffer, BUFFER_SIZE);
			memcpy(buffer, now, sizeof(str));

			iResult = send(ConnectSocket, buffer, BUFFER_SIZE, 0);
			if (iResult > 0)
			{
				std::cout << "send" << iResult << "bytes: " << now << std::endl;
			}
 		}
		else
		{
			FD_CLR(ConnectSocket, &readfds);
			shoutError(result, ConnectSocket, "select() error\n");
		}
	}

	// 종료
	closesocket(ConnectSocket);
	WSACleanup();
	return 0;
}