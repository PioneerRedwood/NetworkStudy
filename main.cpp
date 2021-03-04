#include "predef.h"

std::string SERVER_ADDRESS = "";

#define SERVER

void ClearString(char* buf)
{
	for (int i = 0; i < 32; ++i)
	{
		buf[i] = (buf[i] == '\n' || buf[i] == '\r') ? 0 : buf[i];
	}
}

int main()
{
	printf("Load config\n");

	FILE* fp;
	errno_t err;

	err = fopen_s(&fp, "config.txt", "rt");
	if (err != 0)
	{
		return 0;
	}
	else
	{

		if (fp)
		{
			char addr[32] = { 0, };
			fgets(addr, sizeof(addr), fp);
			ClearString(addr);
			SERVER_ADDRESS = std::string(addr);
			printf("Server address: %s\n", SERVER_ADDRESS.c_str());

			err = fclose(fp);
			if (err != 0)
			{
				return 0;
			}
		}
		else
		{
			printf("Not found config file.\n");
		}
	}
}