#pragma once
#include "Core.h"

#include "Window.h"
#include "VoxelEngine/Core/LayerStack.h"
#include "VoxelEngine/Events/Event.h"
#include "VoxelEngine/Events/ApplicationEvent.h"

#include "VoxelEngine/Core/Timestep.h"

#include "VoxelEngine/ImGui/ImGuiLayer.h"
namespace VoxelEngine {
	const int TICKS_PER_SECOND = 30;
	const float SECONDS_PER_TICK = 1.0f / TICKS_PER_SECOND;
	class Application
	{
	public:
		Application();
		virtual ~Application();
		void Run();
		void Close();
		void OnEvent(Event& e);
		template<typename T>
			requires(std::is_base_of_v<Layer, T>)
		void PushLayer() {
			m_LayerStack.PushLayer<T>();
		}
		template<typename T>
			requires(std::is_base_of_v<Layer, T>)
		void PushOverlay() {
			m_LayerStack.PushOverlay<T>();
		}
		template<typename T>
			requires(std::is_base_of_v<Layer, T>)
		T* GetLayer() {
			return m_LayerStack.GetLayer<T>();
		}
		inline Window& GetWindow() { return *m_Window; }
		static inline Application& Get() { return *s_Instance; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		static Application* s_Instance;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;
		float m_TimeSinceLastTick = 0.0f;
	};
	// To be defined in CLIENT
	Application* CreateApplication();
}
