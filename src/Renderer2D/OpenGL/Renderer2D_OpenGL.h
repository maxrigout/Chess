#pragma once

#include "Renderer2D/Renderer2D.h"

#include <string>
#include <functional>
#include <set>
#include <vector>
#include <unordered_map>

#define N_QUAD_VERTICES 4

class Renderer2D_OpenGL : public Renderer2D
{
public:
	static void LoadOpenGLLibrary(void*(procAddr)(const char*));

	virtual ~Renderer2D_OpenGL() {}
	virtual void OnRenderEnd(std::function<void(void)>&& fn) = 0;
};