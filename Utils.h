#ifndef _UTILS_
#define _UTILS_

#include "predef.h"

#include <iostream>
#include <memory>
#include <errno.h>
#include <stdio.h>

#include <direct.h>
#include <time.h>

namespace Utils
{
	static std::wstring		StringToPCWSTR(const std::string& str)
	{
		int len;
		int strLength = (int)str.length() + 1;
		len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), strLength, 0, 0);
		wchar_t* buf = new wchar_t[len];
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), strLength, buf, len);
		std::wstring r(buf);
		delete[] buf;

		return r;
	}

	static std::string		GetCurrentDateTime()
	{
		char			cTimeStamp[256] = { 0, };
		struct timeval	tv {};
		struct tm		tm;

		//////////////////////////////////////////////
		// �˸�: gettimeofday �Լ��� linux�� �ִ� ������� sys/time.h�� ����
		// ����� Windows ȯ���̹Ƿ� �ش� �޼��带 �����Ѵ�
		// https://stackoverflow.com/questions/10905892/equivalent-of-gettimeday-for-windows
		// Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
		// This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
		// until 00:00:00 January 1, 1970 
		static const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

		SYSTEMTIME	systemTime;
		FILETIME	fileTime;
		uint64_t	time;

		GetSystemTime(&systemTime);
		SystemTimeToFileTime(&systemTime, &fileTime);
		time = ((uint64_t)fileTime.dwLowDateTime);
		time += ((uint64_t)fileTime.dwHighDateTime) << 32;

		tv.tv_sec = (long)((time - EPOCH) / 10000000L);
		tv.tv_usec = (long)(systemTime.wMilliseconds * 1000);
		//////////////////////////////////////////////
		// end of gettimeofday(struct timeval* tv, struct timezone* tz) // timezone�� ���̻� ��� ����

		localtime_s(&tm, (const time_t*)&tv.tv_sec);

		sprintf_s(cTimeStamp, "%04d-%02d-%02d %02d:%02d:%02d.%03d\n",
			tm.tm_year + 1900,
			tm.tm_mon + 1,
			tm.tm_mday,
			tm.tm_hour,
			tm.tm_min,
			tm.tm_sec,
			(int)(tv.tv_usec / 1000)
		);

		return cTimeStamp;
	}

	template<typename ...Args>
	static std::string		GetStringFormat(const std::string& format, Args ...args)
	{
		// snprintf�� �����ڸ� ������ ũ�⸦ �������Ƿ� 1�� ��������
		// �������ڷ� ���޵� �Ű������� ũ�⸦ �˾Ƴ�
		// snprintf(char* buffer, size_t size, const char* format, argument-list)
		// https://www.ibm.com/support/knowledgecenter/ko/ssw_ibm_i_73/rtref/snprintf.htm
		size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1;
		std::unique_ptr<char[]> buffer(new char[size]);
		snprintf(buffer.get(), size, format.c_str(), args ...);

		// string(const char* begin, const char* end) �ּҸ� �Ѱ��༭ ����
		return std::string(buffer.get(), buffer.get() + size - 1);
	}

};

#endif