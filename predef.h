#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <deque>

#include <WinSock2.h>
#include <Windows.h>

#include "MemDB.h"

#define PORT 8888
#define SOCKET_BUFFER 4096


#ifdef WIN32
#pragma comment(lib, "ws2_32.lib")
#endif // WIN32

extern std::string SERVER_ADDRESS;