#include "TCP_Socket.h"

TCP_Socket::TCP_Socket()
{
	sock = -1;

	clientAddr.sin_family = AF_INET;
	clientAddr.sin_port = htons(PORT);
	clientAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
}

TCP_Socket::~TCP_Socket()
{
}

void TCP_Socket::Init()
{
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		Logger::Log(LOG_ERROR, "Could not create socket. error\n");
		return;
	}

	std::string serverAdd = MemDB::GetInstance()->GetValue("serveraddress");
	if (serverAdd == "")
	{
		serverAdd = serveraddress;
	}

	Logger::Log(LOG_INFO, "Server Address: %s\n", serverAdd.c_str());
	
	memset((void*)&serverAddr, 0x00, sizeof(serverAddr));

	serverAddr.sin_family = AF_INET;
	InetPtonW(AF_INET, Utils::StringToPCWSTR(serverAdd).c_str(), &serverAddr.sin_addr.S_un.S_addr);
	serverAddr.sin_port = htons(PORT);

	int flag = 1;
	int ret = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag));
	if (ret == -1)
	{
		Logger::Log(LOG_ERROR, "setsockopt failed. error\n");
		return;
	}
}

bool TCP_Socket::Connect()
{
	int err = ::connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (err < 0)
	{
		Logger::Log(LOG_ERROR, "connect fialed. error\n");
		return false;
	}
	return true;
}

int TCP_Socket::Update(char* buffer)
{	
	return 1;
}

int TCP_Socket::Send(const char* buffer)
{
	return ::send(sock, buffer, TCP_BUFFER, 0);
}

int TCP_Socket::Recv(char* buffer)
{
	return ::recv(sock, buffer, TCP_BUFFER, 0);
}