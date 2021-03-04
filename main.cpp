#include "predef.h"

std::string SERVER_ADDRESS = "";

#define SERVER

int main()
{
#ifndef SERVER
	std::cout << "This is server side." << std::endl;
#else
	std::cout << "This is client side." << std::endl;
#endif

}