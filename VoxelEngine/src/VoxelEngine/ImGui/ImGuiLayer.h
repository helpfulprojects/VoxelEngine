#pragma once
#include <imgui.h>
#include "VoxelEngine/Layer.h"
#include "VoxelEngine/Events/MouseEvent.h"
#include "VoxelEngine/Events/KeyEvent.h"
#include "VoxelEngine/Events/ApplicationEvent.h"
namespace VoxelEngine {
	class VE_API ImGuiLayer :public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();
		void OnAttach();
		//ImGuiKey ImGui_ImplGlfw_KeyToImGuiKey(int keycode, int scancode);
		void OnDetach();
		void OnUpdate();
		void OnEvent(Event& event);
	private:
		static ImGuiKey GlfwKeyToImGuiKey(int keycode);
		static bool IsKeyDownForImGuiIo(ImGuiIO& io, int keycode);
		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
		bool OnMouseMovedEvent(MouseMovedEvent& e);
		bool OnMouseScrolledEvent(MouseScrolledEvent& e);
		bool OnKeyPressedEvent(KeyPressedEvent& e);
		bool OnKeyReleasedEvent(KeyReleasedEvent& e);
		bool OnKeyTypedEvent(KeyTypedEvent& e);
		bool OnWindowResizeEvent(WindowResizeEvent& e);
	private:
		float m_Time = 0.0f;
	};
}
