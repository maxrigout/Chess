#pragma once

#include "Core/Impl/SDL/Window_SDL.h"
#include "Core/Config/SystemConfiguration.h"

class WindowFactory
{
public:
	static Window* CreateWindow(const std::string& type = SystemConfiguration::GetWindowingSystem())
	{
		if (type == "SDL") return new Window_SDL();
		return nullptr;
	}

private:
};