workspace "VoxelEngine"
	architecture "x64"
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
	startproject "MinecraftClone"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "VoxelEngine/vendor/GLFW/include"
IncludeDir["GLAD"] = "VoxelEngine/vendor/GLAD/include"
IncludeDir["ImGui"] = "VoxelEngine/vendor/ImGui"
IncludeDir["glm"] = "VoxelEngine/vendor/glm"

group "Dependencies"
	include "VoxelEngine/vendor/GLFW"
	include "VoxelEngine/vendor/GLAD"
	include "VoxelEngine/vendor/imgui"
group ""


project "VoxelEngine"
	location "VoxelEngine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	buildoptions{"/utf-8"}
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "VoxelEngine/src/pch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",

	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.GLAD}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
	}

	links{
		"GLFW",
		"GLAD",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"VE_PLATFORM_WINDOWS",
			"VE_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}
	filter "configurations:Debug"
		defines {"VE_DEBUG"}
		runtime "Debug"
		symbols "on"
	filter "configurations:Release"
		defines {"VE_RELEASE"}
		runtime "Release"
		optimize "on"
	filter "configurations:Dist"
		defines {"VE_DIST"}
		runtime "Release"
		optimize "on"

project "MinecraftClone"
	location "MinecraftClone"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	buildoptions{"/utf-8"}
	staticruntime "on"

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
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"VE_PLATFORM_WINDOWS"
		}
	links 
	{
		"VoxelEngine",
	}
	filter "configurations:Debug"
		defines {"VE_DEBUG"}
		runtime "Debug"
		symbols "on"
	filter "configurations:Release"
		defines {"VE_RELEASE"}
		runtime "Release"
		optimize "on"
	filter "configurations:Dist"
		defines {"VE_DIST"}
		runtime "Release"
		optimize "on"
