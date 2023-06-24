#pragma once
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
		return s_backendType;
	}
private:
	static RendererBackendType s_backendType;
};