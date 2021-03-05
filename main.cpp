#include "predef.h"

#include "SimpleSocket.h"
#include "TCP_Socket.h"
#include <thread>

std::string serveraddress = "";

void LoadConfig()
{
	printf("Load config\n");

	FILE* fp;
	errno_t err;

	err = fopen_s(&fp, "config.txt", "rt");
	if (err != 0)
	{
		return;
	}
	else
	{

		if (fp)
		{
			char addr[32] = { 0, };
			fgets(addr, sizeof(addr), fp);

			for (int i = 0; i < 32; ++i)
			{
				addr[i] = (addr[i] == '\n' || addr[i] == '\r') ? 0 : addr[i];
			}

			serveraddress = std::string(addr);
			printf("Server address: %s\n", serveraddress.c_str());

			err = fclose(fp);
			if (err != 0)
			{
				return;
			}
		}
		else
		{
			printf("Not found config file.\n");
		}
	}
}

int main()
{
	LoadConfig();

	Socket* serverSocket = new Socket();
	const char* hello = "hello";

	if (serverSocket->Init())
	{
		std::cout << "Server started.. " << std::endl;
		auto sendingPacket = [](bool send, Socket* socket, const char* message)
		{
			while (send)
			{
				socket->SendPacket(message, sizeof(hello));
				send = socket->Update();
			}
		};

		std::thread sendingThread(sendingPacket, serverSocket->Update(), serverSocket, hello);

		char line[16] = { 0, };
		std::cout << "> ";
		std::cin.getline(line, 16);
		while (strcmp(line, "q") != 0)
		{
			std::cout << "> ";
			std::cin.getline(line, 16);
		}

		sendingThread.join();
		std::cout << "exit..\n";
	}

	return 0;
}