workspace "VoxelEngine"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "VoxelEngine/vendor/GLFW/include"
include "VoxelEngine/vendor/GLFW"

project "VoxelEngine"
	location "VoxelEngine"
	kind "SharedLib"
	language "C++"
	buildoptions{"/utf-8"}

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "VoxelEngine/src/pch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}"
	}

	links{
		"GLFW",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"VE_PLATFORM_WINDOWS",
			"VE_BUILD_DLL"
		}
		postbuildcommands 
		{ 
			("{COPYFILE} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/MinecraftClone")
		}

	filter "configurations:Debug"
		defines {"VE_DEBUG"}
		symbols "On"
	filter "configurations:Release"
		defines {"VE_RELEASE"}
		optimize "On"
	filter "configurations:Dist"
		defines {"VE_DIST"}
		optimize "On"

project "MinecraftClone"
	location "MinecraftClone"
	kind "ConsoleApp"
	language "C++"
	buildoptions{"/utf-8"}

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"VoxelEngine/vendor/spdlog/include",
		"VoxelEngine/src"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"VE_PLATFORM_WINDOWS"
		}
	links 
	{
		"VoxelEngine"
	}
	filter "configurations:Debug"
		defines {"VE_DEBUG"}
		symbols "On"
	filter "configurations:Release"
		defines {"VE_RELEASE"}
		optimize "On"
	filter "configurations:Dist"
		defines {"VE_DIST"}
		optimize "On"
