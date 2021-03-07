#include "Network.h"
#include "Commander.h"

Network::Network()
{
	enable = false;

	commander = nullptr;
	socket = nullptr;
}

Network::~Network()
{

}

void	Network::Init()
{
	socket = new SimpleSocket();
	socket->Init();

	commander = new Commander();
}

void	Network::UnInit()
{
	socket->UnInit();
	delete socket;
	socket = nullptr;

	delete commander;

	enable = false;
}

bool	Network::Connect(int no)
{
	enable = socket->Connect();
	Logger::Log(CLIENT_LOG_INFO, "socket connect: %d: %s", no, enable ? "success" : "fail");

	return enable;
}

void	Network::Update()
{
	if (enable)
	{
		if (!socket->Update())
		{
			Logger::Log(CLIENT_LOG_ERROR, "socket disconnect");
			enable = false;
		}
		Read();
	}
}

bool	Network::Read()
{
	if (enable)
	{
		SocketBuffer buffer;
		if (socket->RecvPacket(&buffer))
		{
			ParsePacket(&buffer);
		}
	}
	else
	{
		return false;
	}

	return true;
}

bool	Network::Write(char packet, const char* data, int dataSize)
{
	if (enable)
	{
		// 패킷의 형태
		// |||||||| |||||||| |||||||| |||||||| |||||||| |||||||| |||||||| |||||||| ...
		// | - - - - - H E A D E R - - - - - | | FLAG | | - - - - - - D A T A  - - ...

		char buffer[SOCKET_BUFFER] = {0, };
		memcpy(buffer, &dataSize, sizeof(int));				// dataSize 는 패킷 데이터 크기

		buffer[4] = packet;
		memcpy(buffer+sizeof(int)+sizeof(char), data, dataSize);
		return socket->SendPacket(buffer, dataSize + sizeof(int) + sizeof(char));
		
	}
	return false;
}

int		Network::SendImmediate(const char* data, int dataSize)
{
	return socket->SendImmediate(data, dataSize);
}

void	Network::ParsePacket(SocketBuffer* buffer)
{
	int dataSize = (int&)*(buffer->buffer);
	char packet = (char&)*(buffer->buffer+sizeof(int));

	commander->AddCommand(packet, buffer->buffer + sizeof(int) + sizeof(char), dataSize);
}