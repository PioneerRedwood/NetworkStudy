#ifndef _COMMANDER_
#define _COMMANDER_

#include "predef.h"
#include <vector>

struct Command
{
	char packet;
	char data[SOCKET_BUFFER];

	Command()
	{
		packet = 0;
		memset(data, 0, SOCKET_BUFFER);
	}
};

class Commander
{
public:
	Commander();
	~Commander();

	void					AddCommand(char packet, char *data, int dataLen);
	std::vector<Command>	commandList;
};

#endif // !_COMMANDER_
