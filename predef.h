#ifndef _PREDEF_
#define _PREDEF_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <deque>

#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>

#include "MemDB.h"
#include "Logger.h"
#include "Utils.h"

#define SERVER	1
#define PORT	8888

#ifdef WIN32
#pragma comment(lib, "ws2_32.lib")
#endif // WIN32

//////////////////////////////////////////////////////////////// LOGGING

#define LOG_ERROR				-1
#define LOG_INFO				1

//////////////////////////////////////////////////////////////// LOGGING




//////////////////////////////////////////////////////////////// NON-BLOCKING

#define	USE_NONBLOCK			1

#define	PATCH_DOWNLOAD_DIR		"./tempdownload"
#define	PATCH_SERVER_ADDRESS	"http://127.0.0.1:8000/patch/patch.xml"
#define	PATCH_FILENAME			"patch.xml"

#define	SERVER_ADDRESS			"192.168.0.6"
#define	SERVER_PORT				8888
#define	SERVER_UDP_PORT			11000
#define SOCKET_BUFFER			4096
#define	TCP_BUFFER				8
#define	UDP_BUFFER				8

//////////////////////////////////////////////////////////////// NON-BLOCKING


//////////////////////////////////////////////////////////////// NETWORK PACKET

// RESPONSE PACKET
#define RESPONSE_OK				0x05
#define RESPONSE_FAIL			0x06

// LOG PACKET
#define	CLIENT_LOG_INFO			0x0a
#define CLIENT_LOG_WARN			0x0b
#define CLIENT_LOG_ERROR		0x0c

// PACKET
#define PACKET_TRY_CONNECT		0x05		// connect to server

//////////////////////////////////////////////////////////////// NETWORK PACKET


extern std::string serveraddress;

#endif // !_PREDEF_
