#ifndef _LOGGER_
#define _LOGGER_

#include <stdio.h>

class Logger
{
public:
	static void log(int errNo, const char* format, ...);

private:
	Logger();
	~Logger();
};

#endif
