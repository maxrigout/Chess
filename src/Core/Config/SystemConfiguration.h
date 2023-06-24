#pragma once
#include <string>

class SystemConfiguration
{
public:
	static const char* GetPlatform()
	{
#ifdef __linux__
		return "Linux";
#elif defined _WIN32
		return "Windows";
#elif defined __APPLE__
		return "MacOS";
#endif
	}
	static const char* GetWindowingSystem()
	{
		return "SDL";
	}
private:

};