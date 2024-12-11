#pragma once

#include "Renderer2D/Renderer2D.h"

#include <functional>

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#endif

#define N_QUAD_VERTICES 4

class Renderer2D_OpenGL : public Renderer2D
{
public:
	static void LoadOpenGLLibrary(void*(*procAddr)(const char*));
	// static void LoadOpenGLLibrary(GLFWglproc(*procAddr)(const char*));

	virtual ~Renderer2D_OpenGL() {}
	virtual void OnRenderEnd(std::function<void(void)>&& fn) = 0;
};