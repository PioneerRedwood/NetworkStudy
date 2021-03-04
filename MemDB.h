#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <string>

class MemDB
{
public:
	static MemDB* instance;
	static MemDB* GetInstance()
	{
		if (instance == nullptr)
		{
			instance = new MemDB();
		}
		return instance;
	}

	void Reset();

	bool			GetBoolValue(std::string key, bool d = false);
	int				GetIntValue(std::string key, int d = 0);
	float			GetFloatValue(std::string key, float d = 0.f);
	std::string		GetValue(std::string key);

	void			SetValue(std::string key, int value);
	void			SetValue(std::string key, float vlaue);
	void			SetValue(std::string key, std::string value);

private:	
	MemDB();
	~MemDB();

	std::map<std::string, std::string> keyValue;
};