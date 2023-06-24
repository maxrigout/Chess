#pragma once
#include <string>

enum class RendererBackendType
{
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
		return RendererBackendType::OpenGL;
	}
private:

};