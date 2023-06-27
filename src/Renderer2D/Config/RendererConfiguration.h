#pragma once
#include "Core/Config/ConfigReader.h"
#include "Renderer2D/RendererBackendType.h"
#include <string>

class RendererConfiguration
{
public:
	static void Init()
	{
		s_backendType = BackendTypeStringToEnum(ConfigReader::GetFile("config.yml")["renderer-backend"].GetStringValue());
		s_openglRenderMode = ConfigReader::GetFile("config.yml")["opengl-render-mode"].GetStringValue();
	}
	static RendererBackendType GetRendererBackendType()
	{
		return s_backendType;
	}
	static std::string GetOpenGLRenderMode()
	{
		return s_openglRenderMode;
	}
private:
	RendererConfiguration() {}
	
	static RendererBackendType BackendTypeStringToEnum(const std::string& backendType)
	{
		if (backendType == "Unspec") return RendererBackendType::Unspec;
		else if (backendType == "OpenGL") return RendererBackendType::OpenGL;
		else if (backendType == "Vulkan") return RendererBackendType::Vulkan;
		else if (backendType == "Metal") return RendererBackendType::Metal;
		else if (backendType == "SDL2") return RendererBackendType::SDL2;
		return RendererBackendType::Unknown;
	}

	static RendererBackendType s_backendType;
	static std::string s_openglRenderMode;
};

#ifdef RENDERER_CONFIG_IMPL
#undef RENDERER_CONFIG_IMPL
RendererBackendType RendererConfiguration::s_backendType;
std::string RendererConfiguration::s_openglRenderMode;
#endif