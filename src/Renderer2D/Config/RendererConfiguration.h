#pragma once
#include "Core/Config/ConfigReader.h"
#include "Renderer2D/RendererBackendType.h"
#include <string>

class OpenGLSettings
{
public:
	static void Init(const ConfigReader::Property& props)
	{
		s_renderMode = props["render-mode"].GetStringValue();
	}

	static const std::string& GetRenderMode()
	{
		return s_renderMode;
	}

private:
	static std::string s_renderMode;
};

class RendererConfiguration
{
public:
	static void Init(const ConfigReader::Property& props)
	{
		s_backendType = BackendTypeStringToEnum(props["renderer-backend"].GetStringValue());
		s_openGLSettings.Init(props["opengl-settings"]);
	}
	static RendererBackendType GetRendererBackendType()
	{
		return s_backendType;
	}
	static const OpenGLSettings& GetOpenGLSettings()
	{
		return s_openGLSettings;
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
	static OpenGLSettings s_openGLSettings;
};

#ifdef RENDERER_CONFIG_IMPL
#undef RENDERER_CONFIG_IMPL
RendererBackendType RendererConfiguration::s_backendType;
OpenGLSettings RendererConfiguration::s_openGLSettings;
std::string OpenGLSettings::s_renderMode;
#endif