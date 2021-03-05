#include <stdarg.h>
#include <stdlib.h>

#include "predef.h"
#include "Logger.h"
#include "Utils.h"


void Logger::Log(int errNo, const char* format, ...)
{
	bool isConsoleLogEnable = true;
	bool isFileLogEnable = MemDB::GetInstance()->GetBoolValue("filelog");

	char buffer[2048] = { 0, };
	va_list argueList;
	va_start(argueList, format);
	vsprintf_s(buffer, format, argueList);
	va_end(argueList);

	std::string dateTime = Utils::GetCurrentDateTime();
	std::string logContent;

	// errNo: -1(LOG_ERROR) / 0(LOG_INFO) / ...
	switch (errNo)
	{
	case -1:
		logContent = Utils::GetStringFormat("Error[%d] ", errNo) + dateTime + buffer + '\n';
		break;
	case 0:
		logContent = Utils::GetStringFormat("LOG_INFO") + dateTime + buffer + '\n';
		break;
	default:
		logContent = dateTime + buffer + '\n';
		break;
	}

	if (isFileLogEnable)
	{
		FILE* fileStream;
		errno_t err;

		err = fopen_s(&fileStream, "LOG.txt", "at");
		if (err != 0)
		{
			// log? or console write?
			return;
		}

		if (fileStream)
		{
			fputs(logContent.c_str(), fileStream);

			err = fclose(fileStream);
			if (err != 0)
			{
				// log? or console write?
				return;
			}
		}
	}

	if (isConsoleLogEnable)
	{
		printf(logContent.c_str());
	}

	// 네트워크 클래스 제작 3/4
}