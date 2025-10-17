#include <VoxelEngine.h>
#include <VoxelEngine/Core/EntryPoint.h>
#include "SimpleExamples/SimpleExample6.h"
class MinecraftClone : public VoxelEngine::Application {
public:
	MinecraftClone() {
		PushLayer<SimpleExample6>();
	}
	~MinecraftClone() {

	}
};
VoxelEngine::Application* VoxelEngine::CreateApplication() {
	return new MinecraftClone();
}
