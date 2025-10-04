#pragma once
#include <memory>

#ifdef VE_PLATFORM_WINDOWS
#if VE_DYNAMIC_LINK
#ifdef VE_BUILD_DLL
#define VE_API __declspec(dllexport)
#else
#define VE_API __declspec(dllimport)
#endif
#else
#define VE_API
#endif
#else
#error Voxel Engine only supports Windows!
#endif

#ifdef VE_DEBUG
#define VE_ENABLE_ASSERTS
#define TRACY_ENABLE
#endif

#ifdef VE_ENABLE_ASSERTS
#define VE_ASSERT(x, ...) {if(!(x)){VE_ERROR("Assertion Failed: {0}",__VA_ARGS__); __debugbreak();}}
#define VE_CORE_ASSERT(x, ...) {if(!(x)){VE_CORE_ERROR("Assertion Failed: {0}",__VA_ARGS__); __debugbreak();}}
#else
#define VE_ASSERT(x, ...)
#define VE_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define VE_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace VoxelEngine {
	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T>
	using Ref = std::shared_ptr<T>;
}
