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
	-- intdir = "obj/%{cfg.buildcfg}_%{cfg.architecture}"
	-- outputdir = "bin/%{cfg.buildcfg}_%{cfg.architecture}"
	intdir = os.getenv("PREMAKE_INT_DIR")
	outputdir = os.getenv("PREMAKE_OUT_DIR")

project "Chess"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"

	objdir(intdir .. "/%{prj.name}/%{cfg.buildcfg}")
	targetdir(outputdir .. "/%{prj.name}/%{cfg.buildcfg}")

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
		"deps/include/SDL2",
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
		includedirs
		{
			"C:/VulkanSDK/1.3.250.1/Include"
		}
		libdirs
		{
			"deps/lib",
			"C:/VulkanSDK/1.3.250.1/Lib"
		}
		links
		{
			"SDL2.lib",
			"SDL2main.lib",
			"SDL2_ttf.lib",
			"SDL2_image.lib",
			"vulkan-1.lib"
		}

	filter "system:macosx"
		includedirs
		{
			"deps/include/metal-cpp",
			"deps/include/metal-cpp-extensions",
			"~/dev/VulkanSDK/1.3.261.1/macOS/include"
		}

		buildoptions
		{
			
		}

		links
		{
			"vulkan.framework",
			"SDL2.framework",
			"SDL2_ttf.framework",
			"SDL2_image.framework",
			"OpenGL.framework",
			"Cocoa.framework",
			"IOKit.framework",
			"CoreVideo.framework",
			"CoreFoundation.framework",
			"Metal.framework",
			"QuartzCore.framework",
			"Foundation.framework",
			"MetalKit.framework",
			"curl"
		}
		linkoptions
		{
			"-rpath",
			"/Library/Frameworks",
			"-F/Library/Frameworks"
		}

	filter "system:linux"
		-- TODO

-- TODO tests projects
-- project "Tests"
-- 	kind "ConsoleApp"
-- 	language "C++"
-- 	cppdialect "C++17"