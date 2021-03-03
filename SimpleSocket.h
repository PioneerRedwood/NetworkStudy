#include "predef.h"

struct SocketBuffer
{
	int totalSize;
	int currentSize;
	char buffer[SOCKET_BUFFER];

	SocketBuffer()
	{
		totalSize = -1;
		currentSize = 0;
		memset(buffer, 0, SOCKET_BUFFER);
	}
};

class Socket
{
public:
	Socket();
	~Socket();

	bool Init();
	void UnInit();

	bool Connect();
	void CloseSocket();
	bool Update();

	bool SendPacket(char* packet, int size);
	bool RecvPacket(SocketBuffer* buffer);

private:
	void SendDone();
	void RecvDone();

	bool InitServer();
	bool InitClient();

	void CloseServer();
	void CloseClient();

	bool UpdateServer();
	bool UpdateClient();

#if WIN32
	SOCKET sock;
	SOCKET clientSock;
#else
	int sock;
	int clientSock;
#endif

	struct sockaddr_in serverAddr;
	struct sockaddr_in clientAddr;

	std::deque<SocketBuffer> recvBufferList;
	std::deque<SocketBuffer> sendBufferList;

	SocketBuffer sendBuffer;
	SocketBuffer recvBuffer;
};
