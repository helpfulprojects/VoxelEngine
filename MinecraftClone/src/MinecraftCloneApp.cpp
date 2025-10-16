#include <VoxelEngine.h>
#include <VoxelEngine/Core/EntryPoint.h>
#include "SimpleExamples/SimpleExample3.h"
class MinecraftClone : public VoxelEngine::Application {
public:
	MinecraftClone() {
		PushLayer<SimpleExample3>();
	}
	~MinecraftClone() {

	}
};
VoxelEngine::Application* VoxelEngine::CreateApplication() {
	return new MinecraftClone();
}
