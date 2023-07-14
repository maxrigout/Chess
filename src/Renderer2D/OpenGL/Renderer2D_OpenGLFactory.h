#pragma once

#include "Renderer2D/OpenGL/Classic/Renderer2D_OpenGL_Classic.h"
#include "Renderer2D/OpenGL/Batch/Renderer2D_OpenGL_Batch.h"
#include "Renderer2D/OpenGL/Instance/Renderer2D_OpenGL_Instance.h"

#include "Renderer2D/Config/RendererConfiguration.h"

class Renderer2D_OpenGLFactory
{
public:
	static Renderer2D_OpenGL* CreateOpenGLRenderer(const std::string& type = RendererConfiguration::GetOpenGLSettings().GetRenderMode())
	{
		if (type == "classic")
			return new Renderer2D_OpenGL_Classic();
		else if (type == "batch")
			return new Renderer2D_OpenGL_Batch();
		else if (type == "instance")
			return new Renderer2D_OpenGL_Instance();
		return nullptr;
	}
};