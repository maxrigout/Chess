#pragma once
#include "Core/Config/ConfigReader.h"
#include <string>


enum class RendererBackendType
{
	Unknown,
	Unspec,
	OpenGL,
	Vulkan,
	Metal,
	SDL2,
};

class RendererConfiguration
{
public:
	static RendererBackendType GetRendererBackendType()
	{
		// return s_backendType;
		return BackendTypeStringToEnum(ConfigReader::GetFile("config.yml")["renderer-backend"].GetStringValue());
	}
private:
	static RendererBackendType BackendTypeStringToEnum(const std::string& backendType)
	{
		if (backendType == "Unspec") return RendererBackendType::Unspec;
		else if (backendType == "OpenGL") return RendererBackendType::OpenGL;
		else if (backendType == "Vulkan") return RendererBackendType::Vulkan;
		else if (backendType == "Metal") return RendererBackendType::Metal;
		else if (backendType == "SDL2") return RendererBackendType::SDL2;
		return RendererBackendType::Unknown;
	}

	// static RendererBackendType s_backendType;
};