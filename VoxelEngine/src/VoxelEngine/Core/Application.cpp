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
    VE_PROFILE_FUNCTION;
    VE_CORE_ASSERT(!s_Instance, "Application already exists");
    s_Instance = this;
    m_Window = std::unique_ptr<Window>(Window::Create());
    m_Window->SetEventCallback(VE_BIND_EVENT_FN(Application::OnEvent));

    Renderer::Init();

    PushLayer<ImGuiLayer>();
    m_ImGuiLayer = GetLayer<ImGuiLayer>();
}

Application::~Application() { VE_PROFILE_FUNCTION; }
void Application::Run()
{
    VE_PROFILE_FUNCTION;
    while (m_Running) {
        VE_PROFILE_SCOPE("RunLoop");
        float time = (float)glfwGetTime(); // Platform::GetTime
        Timestep timestep = std::clamp(time - m_LastFrameTime, 0.001f, 0.1f);
        m_LastFrameTime = time;

        {
            VE_PROFILE_SCOPE("LayerStack OnUpdate");
            for (const Scope<Layer>& layer : m_LayerStack) {
                layer->OnUpdate(timestep);
            }
        }
        {
            VE_PROFILE_SCOPE("LayerStack OnRender");
            for (const Scope<Layer>& layer : m_LayerStack) {
                layer->OnRender();
            }
        }
        if (m_ImGuiLayer) {
            m_ImGuiLayer->Begin();
            {
                VE_PROFILE_SCOPE("LayerStack OnImGuiRender");
                for (const Scope<Layer>& layer : m_LayerStack) {
                    layer->OnImGuiRender();
                }
            }
            m_ImGuiLayer->End();
        }
        m_Window->OnUpdate();
        FrameMark;
    }
}
void Application::Close() { m_Running = false; }
void Application::OnEvent(Event& e)
{
    VE_PROFILE_FUNCTION;
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(
        VE_BIND_EVENT_FN(Application::OnWindowClose));
    dispatcher.Dispatch<WindowResizeEvent>(
        VE_BIND_EVENT_FN(Application::OnWindowResize));
    for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();) {
        (*--it)->OnEvent(e);
        if (e.Handled()) {
            break;
        }
    }
}
bool Application::OnWindowClose(WindowCloseEvent& e)
{
    VE_PROFILE_FUNCTION;
    m_Running = false;
    return true;
}
bool Application::OnWindowResize(WindowResizeEvent& e)
{
    VE_PROFILE_FUNCTION;
    RenderCommand::SetViewport(0, 0, e.GetWidth(), e.GetHeight());
    return false;
}
} // namespace VoxelEngine
