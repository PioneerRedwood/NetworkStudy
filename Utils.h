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
		SYSTEMTIME		systemTime;

		GetLocalTime(&systemTime);

		sprintf_s(cTimeStamp, 256, "%04d-%02d-%02d %02d:%02d:%02d ",
			systemTime.wYear,
			systemTime.wMonth,
			systemTime.wDay,
			systemTime.wHour,
			systemTime.wMinute,
			systemTime.wSecond
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
		size_t size = (size_t)snprintf(nullptr, 0, format.c_str(), args ...) + 1;
		std::unique_ptr<char[]> buffer(new char[size]);
		snprintf(buffer.get(), size, format.c_str(), args ...);

		// string(const char* begin, const char* end) �ּҸ� �Ѱ��༭ ����
		return std::string(buffer.get(), buffer.get() + size - 1);
	}

};

#endif