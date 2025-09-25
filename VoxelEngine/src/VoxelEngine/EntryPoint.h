#pragma once

#ifdef VE_PLATFORM_WINDOWS
extern VoxelEngine::Application* VoxelEngine::CreateApplication();
int main(int argc, char** argv) {
	VoxelEngine::Log::Init();
	VE_CORE_WARN("Initialized Log again!");
	VE_INFO("Hello again!");
	auto app = VoxelEngine::CreateApplication();
	app->Run();
	delete app;
}
#endif
