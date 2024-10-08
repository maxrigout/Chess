#pragma once
#include "Core/Config/ConfigReader.h"
#include <string>

class SystemConfiguration
{
public:
	static void Init(const ConfigReader::Property& props)
	{
		s_windowingSystem = props["windowing-system"].GetStringValue();
		s_secretsFolder = props["secrets-folder"].GetStringValue();
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

	static const std::string& GetSecretsFolder()
	{
		return s_secretsFolder;
	}

private:
	SystemConfiguration() {}
	static std::string s_windowingSystem;
	static std::string s_secretsFolder;
};

#ifdef SYS_CONFIG_IMPL
#undef SYS_CONFIG_IMPL
std::string SystemConfiguration::s_windowingSystem;
std::string SystemConfiguration::s_secretsFolder;
#endif