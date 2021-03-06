// This code refered
// https://gist.github.com/Gydo194/c14e52701289354ab66359c2a75706f8

#include "NewServer.h"

Server::Server()
{
	Setup(PORT);
}

Server::Server(int port)
{
	Setup(port);
}

Server::Server(const Server& original)
{
	
}

Server::~Server()
{
	closesocket(masterSocket);
}

void Server::Setup(int port)
{
	masterSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (masterSocket < 0)
	{
		perror("[SERVER][ERROR] Socket creation failed");
	}

	FD_ZERO(&masterfds);
	FD_ZERO(&tempfds);

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.S_un.S_addr = htons(INADDR_ANY);

	ZeroMemory(buffer, BUFFER_SIZE);
}

void Server::InitSocket()
{
	int opt = 1;
	int ret = setsockopt(masterSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(int));
	
	if (ret < 0)
	{
		perror("[SERVER][ERROR] setsockopt() failed");
		Shutdown();
	}
}

void Server::BindSocket()
{
	int ret = bind(masterSocket, (struct sockaddr*)&server_addr, sizeof(server_addr));
	
	if (ret < 0)
	{
		perror("[SERVER][ERROR] bind() failed");
	}

	FD_SET(masterSocket, &masterfds);
	maxfd = masterSocket;
}

void Server::StartListen()
{
	int ret = listen(masterSocket, BACKLOG);

	if(ret < 0)
	{
		perror("[SERVER][ERROR] listen() failed");
	}
}

void Server::Shutdown()
{
	closesocket(masterSocket);
}

void Server::HandleNewConnection()
{
	sockaddr_in client;
	int clientSize = sizeof(struct sockaddr_in);

	tempSocket = accept(masterSocket, (struct sockaddr*)&client, &clientSize);
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

			int ret = send(tempSocket, "[SERVER] Connected" , sizeof("[SERVER] Connected"), 0);
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

	newConnectionCallback(tempSocket);
}

void Server::RecvInputFromExisting(int fd)
{
	int ret = recv(fd, buffer, BUFFER_SIZE, 0);
	if (ret <= 0)
	{
		if (ret == 0)
		{
			disconnectCallback((USORT) fd);
			closesocket(fd);
			FD_CLR(fd, &masterfds);
			return;
		}
		else
		{
			perror("[SERVER][ERROR] recv() failed");
		}
		closesocket(fd);
		FD_CLR(fd, &masterfds);
		return;
	}
	std::cout << "[SERVER][ERROR] Received " << buffer << std::endl;
	receiveCallback(fd, buffer);
	ZeroMemory(buffer, BUFFER_SIZE);
}

void Server::Loop()
{
	tempfds = masterfds;
	std::cout << "[SERVER][MAX] " << maxfd << std::endl;
	
	int sel = select(maxfd +1, &tempfds, nullptr, nullptr, nullptr);
	if (sel < 0)
	{
		perror("[SERVER][ERROR] select() failed");
		Shutdown();
	}

	for (int i = 0; i <= maxfd; ++i)
	{
		if (FD_ISSET(i, &tempfds))
		{
			// tempfds 중 master가 있으면 새로운 연결 처리
			if (masterSocket == i)
			{
				HandleNewConnection();
			}
			// 아니면 수신한 메시지 처리
			else
			{
				RecvInputFromExisting(i);
			}
		}
	}
}

void Server::Init()
{
	InitSocket();
	BindSocket();
	StartListen();
}

void Server::OnInput(void (*rc)(USORT fd, char* buffer))
{
	receiveCallback = rc;
}

void Server::OnConnect(void(*ncc)(USORT fd))
{
	newConnectionCallback = ncc;
}

void Server::OnDisconnect(void(*odc)(USORT fd))
{
	disconnectCallback = odc;
}

USORT Server::SendMessage(USORT conn, char* messageBuffer)
{
	return send(conn, messageBuffer, sizeof(messageBuffer), 0);
}

USORT Server::SendMessage(USORT conn, const char* messageBuffer)
{
	return send(conn, messageBuffer, sizeof(messageBuffer), 0);
}