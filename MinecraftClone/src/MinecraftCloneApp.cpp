#include <VoxelEngine.h>
class ExampleLayer : public VoxelEngine::Layer {
public:
	ExampleLayer() :Layer("Example") {

	}
	void OnUpdate() override {
		//VE_INFO("ExampleLayer::Update");
	}
	void OnEvent(VoxelEngine::Event& event) override {
		VE_TRACE(event);
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
