// Windows socket 프로그래밍 서버 코드 <다중 연결>

#include<winsock2.h>
#include<ws2tcpip.h>
#include<iostream>
#include <vector>
#include <deque>

#define BUFFER_SIZE				4096
#define INT_PORT				9000
#define STR_PORT				"9000"
#define BACKLOG					10

////////////////////////////////////////////////////

#define LOG_CONNECTION			"Connected"
#define MAX_CONNECTION_DELAY	5

#pragma comment (lib, "ws2_32.lib")		

using namespace std;

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

class ClientInfo
{
public:
	unsigned clientId;
	sockaddr_in clientAddr;
	char host[NI_MAXHOST];
	char service[NI_MAXSERV];

	ClientInfo()
	{
		clientId = -1;
		clientAddr.sin_family = AF_INET;
		clientAddr.sin_port = INT_PORT;
		clientAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

		ZeroMemory(host, NI_MAXHOST);
		ZeroMemory(service, NI_MAXSERV);
	}

	ClientInfo(unsigned id, sockaddr_in addr, const char* tempHost, const char* tempService)
	{
		clientId = id;
		clientAddr = addr;

		memcpy(host, tempHost, NI_MAXHOST);
		memcpy(service, tempService, NI_MAXSERV);
	}
};

void main()
{
	fd_set masterfds;
	fd_set tempfds;

	FD_ZERO(&tempfds);
	FD_ZERO(&masterfds);

	WSADATA wsadata;
	int iResult;

	char buffer[BUFFER_SIZE] = { 0, };

	iResult = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (iResult != 0)
		shoutError("WSAStartup failed!\n");

	struct addrinfo* result = NULL, hints;

	// 통신 프로토콜 정보 설정
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// 서버 주소와 포트 번호를 등록

	iResult = getaddrinfo(NULL, STR_PORT, &hints, &result);
	if (iResult != 0)
		shoutError("getaddrinfo failed!\n");

	// client 접속을 받을 소켓 생성
	SOCKET masterSocket = INVALID_SOCKET;

	masterSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (masterSocket == INVALID_SOCKET)
		shoutError("socket error!\n");

	// TCP 통신 소켓 설정
	iResult = bind(masterSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
		shoutError(result, masterSocket, "bind() failed!\n");

	FD_SET(masterSocket, &masterfds);
	unsigned maxfd = masterSocket;

	// sock -> Non-Block
	unsigned long arg = 1;
	iResult = ioctlsocket(masterSocket, FIONBIO, &arg);
	if (iResult == SOCKET_ERROR)
	{
		shoutError(result, masterSocket, "ioctlsocket() failed!\n");
	}

	// listen 실행
	if (listen(masterSocket, BACKLOG) == SOCKET_ERROR)
		shoutError(result, masterSocket, "listen() failed!\n");

	cout << "Wating for Connection.. " << endl;

	std::deque<ClientInfo> clients;

	// loop
	while (1)
	{
		tempfds = masterfds;
		int ret = select(maxfd + 1, &tempfds, nullptr, nullptr, nullptr);

		if (ret < 0)
		{
			std::cout << "[SERVER][ERROR] select() failed\n";
			closesocket(masterSocket);
		}

		for (int fd = 0; fd <= maxfd; ++fd)
		{
			if (FD_ISSET(fd, &tempfds))
			{
				// Handle a new connection
				if (masterSocket == fd)
				{
					sockaddr_in client;
					int clientSize = sizeof(struct sockaddr_in);

					int tempSocket = accept(masterSocket, (struct sockaddr*)&client, &clientSize);
					if (tempSocket < 0)
					{
						perror("[SERVER][ERROR] accept() failed");
					}
					else
					{
						FD_SET(tempSocket, &masterfds);

						if (tempSocket > maxfd)
						{
							maxfd = tempSocket;
						}

						// client 접속 내용 전시
						char host[NI_MAXHOST];
						char service[NI_MAXSERV];

						ZeroMemory(host, NI_MAXHOST);
						ZeroMemory(service, NI_MAXSERV);

						if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST,
							service, NI_MAXSERV, 0) == 0)
						{
							std::cout << "[SERVER] Connection on port: " << service << std::endl;

							int ret = send(tempSocket, "[SERVER] Connected", sizeof("[SERVER] Connected"), 0);
							if (ret == SOCKET_ERROR)
							{
								std::cout << "[SERVER][ERROR]ECHO failed\n";
								// ECHO 전송 실패시 오류 메시지
							}
							else
							{
								// 연결 등록 성공 시 접속 클라이언트 정보 저장하기
								//clients.push_back(ClientInfo(countOfClients + 1, client, host, service));
							}
						}
					}
				}
				// Recv from Exsiting
				else
				{
					ret = recv(fd, buffer, BUFFER_SIZE, 0);
					if (ret <= 0)
					{
						if (ret == 0)
						{
							closesocket(fd);
							FD_CLR(fd, &masterfds);
							return;
						}
						else
						{
							perror("[SERVER][ERROR] recv() failed");
							// client와 연결 끊김
						}
						closesocket(fd);
						FD_CLR(fd, &masterfds);
						return;
					}
					std::cout << "[SERVER][RECV] Received " << buffer << std::endl;
					ZeroMemory(buffer, BUFFER_SIZE);
				}
			}
		}
	}
}
