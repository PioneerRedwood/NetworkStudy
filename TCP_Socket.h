#ifndef _TCPSOCKET_
#define _TCPSOCKET_

#include "predef.h"

class TCP_Socket
{
public:
	TCP_Socket();
	~TCP_Socket();

	void	Init();
	bool	Connect();
	int		Update(char* buffer);
	int		Send(const char* buffer);
	int		Recv(char* buffer);

private:
	int sock;
	struct sockaddr_in serverAddr;
	struct sockaddr_in clientAddr;
};

#endif // !_TCPSOCKET_
