#include "pch.h"
#include "WindowsWindow.h"

#include "VoxelEngine/Events/KeyEvent.h"
#include "VoxelEngine/Events/MouseEvent.h"
#include "VoxelEngine/Events/ApplicationEvent.h"


namespace VoxelEngine {
	static bool s_GLFWInitialized = false;
	static void GLFWErrorCallback(int error, const char* description) {
		VE_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}
	Window* Window::Create(const WindowProps& props) {
		return new WindowsWindow(props);
	}
	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		VE_PROFILE_FUNCTION;
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		VE_PROFILE_FUNCTION;
		Shutdown();
	}

	void WindowsWindow::OnUpdate()
	{
		VE_PROFILE_FUNCTION;
		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		if (enabled) {
			glfwSwapInterval(1);
		}
		else {
			glfwSwapInterval(0);
		}
		m_Data.VSync = enabled;
	}

	void WindowsWindow::SetCursorVisibility(bool visible)
	{
		if (visible) {
			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else {
			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}

	void WindowsWindow::ToggleCursorVisibility()
	{
		int mode = glfwGetInputMode(m_Window, GLFW_CURSOR);
		if (mode == GLFW_CURSOR_NORMAL) {
			SetCursorVisibility(false);
		}
		else {
			SetCursorVisibility(true);
		}
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}

	void WindowsWindow::SetMaximized(bool enabled)
	{
		if (enabled) {
			glfwMaximizeWindow(m_Window);
		}
		else {
			glfwRestoreWindow(m_Window);
		}
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		VE_PROFILE_FUNCTION;
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;
		m_Data.lastMouseX = props.Width / 2.0f;
		m_Data.lastMouseY = props.Height / 2.0f;

		VE_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);


		if (!s_GLFWInitialized) {
			int success = glfwInit();
			VE_CORE_ASSERT(success, "Could not initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;
		}
		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
		m_Context = new GraphicsContext(m_Window);
		m_Context->Init();

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		// Set GLFW callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;
			data.lastMouseX = width / 2.0f;
			data.lastMouseY = height / 2.0f;
			WindowResizeEvent event(width, height);
			data.EventCallback(event);
			});
		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
			});
		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			switch (action) {
			case GLFW_PRESS: {
				KeyPressedEvent event(key, 0);
				data.EventCallback(event);
				break;
			}
			case GLFW_RELEASE: {
				KeyReleasedEvent event(key);
				data.EventCallback(event);
				break;
			}
			case GLFW_REPEAT: {
				KeyPressedEvent event(key, 1);
				data.EventCallback(event);
				break;
			}
			}
			});
		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action) {
			case GLFW_PRESS: {
				MouseButtonPressedEvent event(button);
				data.EventCallback(event);
				break;
			}
			case GLFW_RELEASE: {
				MouseButtonReleasedEvent event(button);
				data.EventCallback(event);
				break;
			}
			}
			});
		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
			});
		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			float xOffset = (float)xPos - data.lastMouseX;
			float yOffset = data.lastMouseY - (float)yPos;
			MouseMovedEvent event((float)xPos, (float)yPos, xOffset, yOffset);
			data.lastMouseX = (float)xPos;
			data.lastMouseY = (float)yPos;
			data.EventCallback(event);
			});
		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			KeyTypedEvent event(keycode);
			data.EventCallback(event);
			});

	}

	void WindowsWindow::Shutdown()
	{
		VE_PROFILE_FUNCTION;
		glfwDestroyWindow(m_Window);
	}
}

