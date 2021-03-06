// This code refered
// https://gist.github.com/Gydo194/c14e52701289354ab66359c2a75706f8

#ifndef NEWSERVER
#define NEWSERVER

#include <stdio.h>
#include <iostream>
#include <WinSock2.h>
#include<ws2tcpip.h>
#include <memory>

#define BUFFER_SIZE 1024
#define PORT		9000
#define USORT		uint16_t
#define BACKLOG		5

class Server
{
public:
	Server();
	Server(int port);
	Server(const Server& original);
	~Server();

	USORT connector_fd;

	void Shutdown();
	void Init();
	void Loop();

	// callback setters
	void OnConnect(void (*ncc)(USORT fd));
	void OnDisconnect(void (*odc)(USORT fd));
	void OnInput(void(*oi)(USORT fd, char* buffer));

	USORT SendMessage(USORT conn, const char* messageBuffer);
	USORT SendMessage(USORT conn, char* messageBuffer);

private:
	fd_set masterfds;
	fd_set tempfds;

	USORT maxfd;

	int masterSocket;
	int tempSocket;

	struct sockaddr_storage client_addr;
	struct sockaddr_in		server_addr;

	char buffer[BUFFER_SIZE];

	void (*newConnectionCallback) (USORT fd);
	void (*receiveCallback)		  (USORT fd, char* buffer);
	void (*disconnectCallback)	  (USORT fd);

	void Setup(int port);
	void InitSocket();
	void BindSocket();
	void StartListen();
	void HandleNewConnection();
	void RecvInputFromExisting(int fd);
};

#endif // !NEWSERVER
