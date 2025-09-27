#include <VoxelEngine.h>
#include "imgui.h"
class ExampleLayer : public VoxelEngine::Layer {
public:
	ExampleLayer() :Layer("Example") {

	}
	void OnUpdate() override {
		if (VoxelEngine::Input::IsKeyPressed(VE_KEY_TAB)) {
			VE_TRACE("Tab key is pressed!");
		}
	}
	void OnEvent(VoxelEngine::Event& event) override {
	}
	void OnImGuiRender() {
		ImGui::Begin("Test");
		ImGui::Text("Hello World");
		ImGui::End();
	}
};
class MinecraftClone : public VoxelEngine::Application {
public:
	MinecraftClone() {
		PushLayer(new ExampleLayer());
	}
	~MinecraftClone() {

	}
};
VoxelEngine::Application* VoxelEngine::CreateApplication() {
	return new MinecraftClone();
}
