#pragma once

#include <unordered_map>
#include <string>
#include <filesystem>
#include <fstream>
#include "Core/Logger.h"

class SecretManager
{
public:
	~SecretManager() = default;

	static void Init(const std::string& basePath)
	{
		auto extractFileName = [](const std::string& filePath)
		{
			size_t pos = filePath.find_last_of('/');
			if (pos == std::string::npos)
				return filePath;
			
			return filePath.substr(pos + 1);
		};

		auto removeExtension = [](const std::string& fileName)
		{
			size_t pos = fileName.find_last_of('.');
			if (pos == std::string::npos)
				return fileName;

			return fileName.substr(0, pos);
		};

		// list all the files in the directory
		// read each file
		// append to the secrets map
		const std::filesystem::path baseSecretFolder(basePath);
		for (const auto& dir_entry : std::filesystem::directory_iterator(baseSecretFolder))
		{
			std::string name = removeExtension((extractFileName(dir_entry.path())));
			std::fstream f(dir_entry.path());
			if (!f.is_open())
			{
				LOG_WARN("unable to open file: ", dir_entry.path());
				continue;
			}

			std::stringstream ss;
			ss << f.rdbuf();
			std::string value;
			ss >> value;

			f.close();

			s_secrets.emplace(name, value);

			LOG_INFO("loaded secret: ", name);
		}
	}

	static const std::string& GetSecret(const std::string& key)
	{
		return s_secrets.at(key);
	}

private:
	SecretManager() = default;

	static std::unordered_map<std::string, std::string> s_secrets;
};

#ifdef SECRET_CONFIG_IMPL
#undef SECRET_CONFIG_IMPL
std::unordered_map<std::string, std::string> SecretManager::s_secrets;
#endif