#pragma once
#include "Core/Config/ConfigReader.h"
#include "Renderer2D/RendererBackendType.h"
#include <string>

class RendererConfiguration
{
public:
	static void Init()
	{
		s_backendType = BackendTypeStringToEnum(ConfigReader::GetFile("config.yml")["renderer"]["renderer-backend"].GetStringValue());
		s_openglRenderMode = ConfigReader::GetFile("config.yml")["renderer"]["opengl-settings"]["opengl-render-mode"].GetStringValue();
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
		else if (backendType == "opengl") return RendererBackendType::OpenGL;
		else if (backendType == "vulkan") return RendererBackendType::Vulkan;
		else if (backendType == "metal") return RendererBackendType::Metal;
		else if (backendType == "sdl2") return RendererBackendType::SDL2;
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