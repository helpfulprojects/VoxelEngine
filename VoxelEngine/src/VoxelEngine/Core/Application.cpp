#include "pch.h"
#include "Application.h"

#include "VoxelEngine/Core/Log.h"

#include "Input.h"
#include "VoxelEngine/Renderer/Renderer.h"
#include <GLFW/glfw3.h>
namespace VoxelEngine {
	Application* Application::s_Instance = nullptr;
	Application::Application()
	{
		VE_PROFILE_FUNCTION();
		VE_CORE_ASSERT(!s_Instance, "Application already exists");
		s_Instance = this;
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(VE_BIND_EVENT_FN(Application::OnEvent));

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer;
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
		VE_PROFILE_FUNCTION();
	}
	void Application::PushLayer(Layer* layer) {
		VE_PROFILE_FUNCTION();
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}
	void Application::PushOverlay(Layer* overlay) {
		VE_PROFILE_FUNCTION();
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}
	void Application::Run()
	{
		VE_PROFILE_FUNCTION();
		while (m_Running) {
			VE_PROFILE_SCOPE("RunLoop");
			float time = (float)glfwGetTime(); // Platform::GetTime
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;
			{
				//VE_PROFILE_SCOPE("LayerStack OnUpdate");
				for (Layer* layer : m_LayerStack) {
					layer->OnUpdate(timestep);
				}
			}
			m_ImGuiLayer->Begin();
			{
				//VE_PROFILE_SCOPE("LayerStack OnImGuiRender");
				for (Layer* layer : m_LayerStack) {
					layer->OnImGuiRender();
				}
			}
			m_ImGuiLayer->End();
			m_Window->OnUpdate();
		}
	}
	void Application::OnEvent(Event& e)
	{
		VE_PROFILE_FUNCTION();
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(VE_BIND_EVENT_FN(Application::OnWindowClose));
		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();) {
			(*--it)->OnEvent(e);
			if (e.Handled()) {
				break;
			}
		}
	}
	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
}
