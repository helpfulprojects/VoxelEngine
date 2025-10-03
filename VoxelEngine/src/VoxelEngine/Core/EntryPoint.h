#pragma once

#ifdef VE_PLATFORM_WINDOWS
extern VoxelEngine::Application* VoxelEngine::CreateApplication();
int main(int argc, char** argv) {
	VoxelEngine::Log::Init();

	VE_PROFILE_BEGIN_SESSION("Startup", "VoxelEngineProfile-Startup.json");
	auto app = VoxelEngine::CreateApplication();
	VE_PROFILE_END_SESSION();
	VE_PROFILE_BEGIN_SESSION("Runtime", "VoxelEngineProfile-Runtim.json");
	app->Run();
	VE_PROFILE_END_SESSION();
	VE_PROFILE_BEGIN_SESSION("Shutdown", "VoxelEngineProfile-Shutdown.json");
	delete app;
	VE_PROFILE_END_SESSION();
}
#endif
