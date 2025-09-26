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
IncludeDir["GLAD"] = "VoxelEngine/vendor/GLAD/include"
IncludeDir["ImGui"] = "VoxelEngine/vendor/ImGui"
include "VoxelEngine/vendor/GLFW"
include "VoxelEngine/vendor/GLAD"
include "VoxelEngine/vendor/imgui"

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
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.GLAD}",
		"%{IncludeDir.ImGui}"
	}

	links{
		"GLFW",
		"GLAD",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"VE_PLATFORM_WINDOWS",
			"VE_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}
		postbuildcommands 
		{ 
			("{COPYFILE} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/MinecraftClone")
		}

	filter "configurations:Debug"
		defines {"VE_DEBUG"}
		buildoptions "/MDd"
		symbols "On"
	filter "configurations:Release"
		defines {"VE_RELEASE"}
		buildoptions "/MD"
		optimize "On"
	filter "configurations:Dist"
		defines {"VE_DIST"}
		buildoptions "/MD"
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
		"VoxelEngine/src",
		"%{IncludeDir.ImGui}"
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
		buildoptions "/MDd"
		symbols "On"
	filter "configurations:Release"
		defines {"VE_RELEASE"}
		buildoptions "/MD"
		optimize "On"
	filter "configurations:Dist"
		defines {"VE_DIST"}
		buildoptions "/MD"
		optimize "On"
