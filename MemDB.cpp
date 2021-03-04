#include <stdio.h>
#include "MemDB.h"

MemDB* MemDB::instance = nullptr;

MemDB::MemDB()
{

}

MemDB::~MemDB()
{

}

void		MemDB::Reset()
{
	keyValue.clear();
}

bool		MemDB::GetBoolValue(std::string key, bool d)
{
	std::string ret = GetValue(key);
	if (ret.empty())
	{
		return d;
	}

	return ret == "true" ? true : false;
}

int			MemDB::GetIntValue(std::string key, int d)
{
	std::string ret = GetValue(key);
	if (ret.empty())
	{
		return d;
	}

	return atoi(ret.c_str());
}

float		MemDB::GetFloatValue(std::string key, float d)
{
	std::string ret = GetValue(key);
	if (ret.empty())
	{
		return d;
	}

	return (float)atof(ret.c_str());
}

void		MemDB::SetValue(std::string key, int value)
{
	char valueStr[256];
#ifdef WIN32
	_snprintf(valueStr, sizeof(valueStr), "%d", value);
#else
	snprintf(valueStr, sizeof(valueStr), "%d", value);
#endif // WIN32
	
	SetValue(key, valueStr);
}

void		MemDB::SetValue(std::string key, float value)
{
	char valueStr[256];
#ifdef WIN32
	_snprintf(valueStr, sizeof(valueStr), "%f", value);
#else
	snprintf(valueStr, sizeof(valueStr), "%f", value);
#endif // WIN32

	SetValue(key, valueStr);
}

std::string	MemDB::GetValue(std::string key)
{
	std::map<std::string, std::string>::iterator iter = keyValue.find(key);
	if (iter != keyValue.end())
	{
		return iter->second;
	}

	return std::string("");
}

void		MemDB::SetValue(std::string key, std::string value)
{
	std::map<std::string, std::string>::iterator iter = keyValue.find(key);
	if (iter == keyValue.end())
	{
		keyValue.insert(std::make_pair(key, value));
		return;
	}

	iter->second = value;
}