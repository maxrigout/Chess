#include "Renderer2D_OpenGL.h"
#include <glad/glad.h>

static bool areGLProcsLoaded = false;

void Renderer2D_OpenGL::LoadOpenGLLibrary(void*(procAddr)(const char*))
{
	if (!areGLProcsLoaded)
	{
		gladLoadGLLoader(procAddr);
		areGLProcsLoaded = true;
	}
}