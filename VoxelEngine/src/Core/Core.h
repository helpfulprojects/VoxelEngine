#pragma once

#ifdef VE_PLATFORM_WINDOWS
#ifdef VE_BUILD_DLL
#define VE_API __declspec(dllexport)
#else
#define VE_API __declspec(dllimport)
#endif
#else
#error Voxel Engine only supports Windows!
#endif
