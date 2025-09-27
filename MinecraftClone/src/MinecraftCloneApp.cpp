#include <VoxelEngine.h>
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
};
class MinecraftClone : public VoxelEngine::Application {
public:
	MinecraftClone() {
		PushLayer(new ExampleLayer());
		PushOverlay(new VoxelEngine::ImGuiLayer());
	}
	~MinecraftClone() {

	}
};
VoxelEngine::Application* VoxelEngine::CreateApplication() {
	return new MinecraftClone();
}
