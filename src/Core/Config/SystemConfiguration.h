#pragma once
#include "Core/Config/ConfigReader.h"
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
	static const std::string& GetWindowingSystem()
	{
		// return s_windowingSystem;
		return ConfigReader::GetFile("config.yml")["windowing-system"].GetStringValue();
	}
private:
	// static std::string s_windowingSystem;
};