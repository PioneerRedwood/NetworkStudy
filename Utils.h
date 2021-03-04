#pragma once

#include "predef.h"

class Utils
{
public:
	static Utils* instance;
	static Utils* GetInstance()
	{
		if (instance == nullptr)
		{
			return new Utils();
		}
		return instance;
	}

	std::wstring StringToPCWSTR(const std::string& str)
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

private:
	Utils();
	~Utils();

};