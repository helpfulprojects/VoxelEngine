#pragma once
#include "Core.h"

#include "Window.h"
#include "VoxelEngine/LayerStack.h"
#include "Events/Event.h"
#include "VoxelEngine/Events/ApplicationEvent.h"

#include "VoxelEngine/ImGui/ImGuiLayer.h"
namespace VoxelEngine {
	class VE_API Application
	{
	public:
		Application();
		virtual ~Application();
		void Run();
		void OnEvent(Event& e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		inline Window& GetWindow() { return *m_Window; }
		static inline Application& Get() { return *s_Instance; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		static Application* s_Instance;
		LayerStack m_LayerStack;
	};
	// To be defined in CLIENT
	Application* CreateApplication();
}
