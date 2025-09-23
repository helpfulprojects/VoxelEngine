#pragma once

#ifdef VE_PLATFORM_WINDOWS
extern VoxelEngine::Application* VoxelEngine::CreateApplication();
int main(int argc, char** argv) {
	auto app = VoxelEngine::CreateApplication();
	app->Run();
	delete app;
}
#endif
