#include "pch.h"
#include "OpenGLContext.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <tracy/TracyOpenGL.hpp>
namespace VoxelEngine {
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) : m_WindowHandle(windowHandle)
	{
		VE_CORE_ASSERT(windowHandle, "Window handle is null!");
	}
	void OpenGLContext::Init()
	{
		VE_PROFILE_FUNCTION();
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		VE_CORE_ASSERT(status, "Failed to initialize Glad!");

		VE_CORE_INFO("OpenGL Info:");
		VE_CORE_INFO("  Vendor: {0}", (const char*)glGetString(GL_VENDOR));
		VE_CORE_INFO("  Renderer: {0}", (const char*)glGetString(GL_RENDERER));
		VE_CORE_INFO("  Version: {0}", (const char*)glGetString(GL_VERSION));
	}
	void OpenGLContext::SwapBuffers()
	{
		VE_PROFILE_FUNCTION();
		glfwSwapBuffers(m_WindowHandle);
		TracyGpuCollect;
	}
}