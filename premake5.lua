workspace "Chess"
	architecture "x64"
	startproject "Chess"

	configurations
	{
		"Debug",
		"Release"
	}

	projectName = "Chess"
	baseDir = ""
	intdir = "obj/%{cfg.buildcfg}_%{cfg.architecture}"
	outputdir = "bin/%{cfg.buildcfg}_%{cfg.architecture}"

project "Chess"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

	objdir(intdir)
	targetdir(outputdir)

	files 
	{
		"src/**.h",
		"src/**.cpp",
		"src/**.c"
	}

	removefiles
	{
		"src/Utils/**"
	}

	includedirs
	{
		"src",
		"deps/include",
		"deps/include/SDL2"
	}

	postbuildcommands
	{
		-- TODO
	}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"
	
	filter "configurations:Release"
		runtime "Release"
		optimize "On"

	filter "system:windows"
		libdirs
		{
			"deps/lib",
		}    
		links
		{
			"SDL2.lib",
			"SDL2main.lib",
			"SDL2_ttf.lib",
			"SDL2_image.lib"
		}

	filter "system:macosx"
		links
		{
			"SDL2.framework",
			"SDL2_ttf.framework",
			"SDL2_image.framework"
		}
		linkoptions {"-F/Library/Frameworks"}

	filter "system:linux"
		-- TODO
project "Tests"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"