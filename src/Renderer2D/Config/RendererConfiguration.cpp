#include "RendererConfiguration.h"
#include "Core/Config/ConfigReader.h"

static RendererBackendType BackendTypeStringToEnum(const std::string& backendType)
{
	if (backendType == "Unspec") return RendererBackendType::Unspec;
	else if (backendType == "OpenGL") return RendererBackendType::OpenGL;
	else if (backendType == "Vulkan") return RendererBackendType::Vulkan;
	else if (backendType == "Metal") return RendererBackendType::Metal;
	else if (backendType == "SDL2") return RendererBackendType::SDL2;
	return RendererBackendType::Unknown;
}

RendererBackendType RendererConfiguration::s_backendType = BackendTypeStringToEnum(ConfigReader::GetFile("config.yml")["renderer-backend"].GetStringValue());