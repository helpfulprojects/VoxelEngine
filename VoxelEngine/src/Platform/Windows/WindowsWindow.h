#pragma once
#include "VoxelEngine/Core/Window.h"
#include <GLFW/glfw3.h>
#include "VoxelEngine/Renderer/GraphicsContext.h"
namespace VoxelEngine {
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();
		void OnUpdate() override;
		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }
		inline virtual float GetAspectRatio() const override { return (float)m_Data.Width / m_Data.Height; };

		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;
		virtual void SetMaximized(bool enabled) override;
		inline virtual void* GetNativeWindow() const override { return m_Window; };
	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
		GLFWwindow* m_Window;
		GraphicsContext* m_Context;
		struct WindowData {
			std::string Title;
			unsigned int Width, Height;
			float lastMouseX, lastMouseY;
			bool VSync;
			EventCallbackFn EventCallback;
		};
		WindowData m_Data;
	};
}

