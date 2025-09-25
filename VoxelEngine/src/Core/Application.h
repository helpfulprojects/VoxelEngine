#pragma once
#include "Core.h"
#include "Core/Events/ApplicationEvent.h"
#include "Events/Event.h"
#include "Window.h"

namespace VoxelEngine {
	class VE_API Application
	{
	public:
		Application();
		virtual ~Application();
		void Run();
		void OnEvent(Event& e);
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
	};
	// To be defined in CLIENT
	Application* CreateApplication();
}
