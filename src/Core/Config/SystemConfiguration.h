#pragma once
#include "Core/Config/ConfigReader.h"
#include <string>

class SystemConfiguration
{
public:
	static void Init()
	{
		s_windowingSystem = ConfigReader::GetFile("config.yml")["system"]["windowing-system"].GetStringValue();
	}
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
		return s_windowingSystem;
	}
private:
	SystemConfiguration() {}
	static std::string s_windowingSystem;
};

#ifdef SYS_CONFIG_IMPL
#undef SYS_CONFIG_IMPL
std::string SystemConfiguration::s_windowingSystem;
#endif