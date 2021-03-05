#ifndef _LOGGER_
#define _LOGGER_

#include <stdio.h>

class Logger
{
public:
	static void Log(int errNo, const char* format, ...);

private:
	Logger()	{};
	~Logger()	{};
};

#endif
