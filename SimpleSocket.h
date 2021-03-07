#ifndef _SOCKET_
#define _SOCKET_

#include "predef.h"

struct SocketBuffer
{
	int totalSize;						// 전체 길이
	int currentSize;					// 진행중인 버퍼 위치 (송/수신 모두 포함)
	char buffer[SOCKET_BUFFER];

	SocketBuffer()
	{
		totalSize = -1;
		currentSize = 0;
		memset(buffer, 0, SOCKET_BUFFER);
	}
};

class SimpleSocket
{
public:
	SimpleSocket();
	~SimpleSocket();

	bool Init();
	void UnInit();

	bool Connect();
	void CloseSocket();
	bool Update();

	bool SendPacket(const char* packet, int size);
	bool RecvPacket(SocketBuffer* buffer);

	int SendImmediate(const char* buffer, int dataSize);

private:
	void SendDone();
	void RecvDone();

	bool InitServer();
	bool InitClient();

	void CloseServer();
	void CloseClient();

	bool UpdateServer();
	bool UpdateClient();

	SOCKET serverSock;
	SOCKET clientSock;

	struct sockaddr_in serverAddr;
	struct sockaddr_in clientAddr;

	std::deque<SocketBuffer> recvBufferList;
	std::deque<SocketBuffer> sendBufferList;

	SocketBuffer sendBuffer;
	SocketBuffer recvBuffer;
};

#endif // !_SOCKET_
