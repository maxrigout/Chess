#include "Renderer2D_OpenGL.h"
#ifndef __APPLE__
#include <glad/glad.h>
#endif

static bool areGLProcsLoaded = false;

void Renderer2D_OpenGL::LoadOpenGLLibrary(void*(*procAddr)(const char*))
{
#ifndef __APPLE__
	if (!areGLProcsLoaded)
	{
		gladLoadGLLoader(procAddr);
		areGLProcsLoaded = true;
	}
#else
	areGLProcsLoaded = true;
#endif
}

// void Renderer2D_OpenGL::LoadOpenGLLibrary(GLFWglproc(*procAddr)(const char*))
// {
// 	if (!areGLProcsLoaded)
// 	{
// 		gladLoadGLLoader(procAddr);
// 		areGLProcsLoaded = true;
// 	}
// }