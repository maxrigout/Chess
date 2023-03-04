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
	intdir = devFolder .. "bin-int/%{cfg.buildcfg}_%{cfg.architecture}"
	outputdir = devFolder .. "bin/%{cfg.buildcfg}_%{cfg.architecture}"

project "Chess"
	-- location "%{prj.name}"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

	objdir(intdir)
	targetdir(outputdir)

	files 
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/deps/include/**.h",
		"%{prj.name}/deps/include/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/deps/include",
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
			"%{prj.name}/deps/libraries",
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
