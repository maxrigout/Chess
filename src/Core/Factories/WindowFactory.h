#pragma once

#include "Core/Impl/SDL/Window_SDL.h"
#include "Core/Config/SystemConfiguration.h"

#include <map>
#include <exception>

class WindowFactory
{
public:
	static Window* CreateWindow(const std::string& type = SystemConfiguration::GetWindowingSystem())
	{
		if (type == "sdl") return new Window_SDL();
		return nullptr;
		// const auto ite = s_constructorMap.find(type);
		// if (ite == s_constructorMap.end())
		// {
		// 	std::string msg = "unknown window type: " + type + ".\nAllowed types are: ";
		// 	std::string tmp;
		// 	for (const auto& [key, _] : s_constructorMap)
		// 	{
		// 		tmp += ", " + key;
		// 	}
		// 	msg += tmp.substr(2);
		// 	throw std::runtime_error(msg);
		// }
		// return ite->second();
	}

private:
	static std::map<std::string, std::function<Window*()>> s_constructorMap;
};