#ifndef _NETWORK_
#define _NETWORK_

#include "SimpleSocket.h"
#include "predef.h"

class Commander;

class Network
{
public:
	Network();
	~Network();

	void		Init();
	void		UnInit();
	bool		Connect(int no);

	void		Update();
	bool		Read();
	bool		Write(char packet, const char* data, int dataSize);

	int			SendImmediate(const char* data, int dataSize);
	bool		GetEnable() { return enable; }

	Commander*	GetCommander() { return commander; }

private:
	void		ParsePacket(SocketBuffer* buffer);

	bool		enable;
	Commander*	commander;
	Socket*		socket;
};

#endif