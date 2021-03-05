#include "Commander.h"

Commander::Commander()
{
}

Commander::~Commander()
{
}

void Commander::AddCommand(char packet, char* data, int dataLen)
{
	Command com;
	com.packet = packet;

	commandList.push_back(com);
}
