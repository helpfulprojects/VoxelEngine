#include "DebugOverlay.h"
#include "../Layers/GameLayer.h"

DebugOverlay::DebugOverlay()
{

}
DebugOverlay::~DebugOverlay() {

}
void DebugOverlay::OnAttach() {
}
void DebugOverlay::OnDetach() {
}
void DebugOverlay::OnUpdate(VoxelEngine::Timestep ts) {
}
void DebugOverlay::OnImGuiRender() {
	if (m_Visible) {
		GameLayer* gameLayer = VoxelEngine::Application::Get().GetLayer<GameLayer>();
		if (gameLayer) {
			VoxelEngine::PerspectiveCamera camera = gameLayer->GetCamera();
			float cameraYaw = camera.GetYaw();
			std::string directionLookingAt = "";
			if (cameraYaw >= -45.0f && cameraYaw < 45.0f) {
				directionLookingAt = "Facing: south (Towards positive Z)";
			}
			else if (cameraYaw >= 45.0f && cameraYaw < 135.0f) {
				directionLookingAt = "Facing: west (Towards negative X)";
			}
			else if (cameraYaw >= 135.0f || cameraYaw < -135.0f) {
				directionLookingAt = "Facing: north (Towards negative Z)";
			}
			else if (cameraYaw >= -135.0f && cameraYaw < 45.0f) {
				directionLookingAt = "Facing: east (Towards positive X)";
			}
			ImGui::Begin("Debug Menu");
			ImGui::Text("%s (%.1f / %.1f)", directionLookingAt.c_str(), camera.GetYaw(), camera.GetPitch());
			ImGui::End();
		}
	}
}
void DebugOverlay::OnEvent(VoxelEngine::Event& event) {
	VoxelEngine::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<VoxelEngine::KeyPressedEvent>([&](VoxelEngine::KeyPressedEvent& e) {
		if (e.GetKeyCode() == VE_KEY_F3) {
			m_Visible = !m_Visible;
			return true;
		}
		return false;
		});
	dispatcher.Dispatch<VoxelEngine::KeyPressedEvent>([&](VoxelEngine::KeyPressedEvent& e) {
		if (e.GetKeyCode() == VE_KEY_TAB) {
			VoxelEngine::Application::Get().GetWindow().ToggleCursorVisibility();
			return true;
		}
		return false;
		});
}
