#include <VoxelEngine.h>
#include <VoxelEngine/Core/EntryPoint.h>
#include "SimpleExamples/SimpleExample4.h"
class MinecraftClone : public VoxelEngine::Application {
public:
	MinecraftClone() {
		PushLayer<SimpleExample4>();
	}
	~MinecraftClone() {

	}
};
VoxelEngine::Application* VoxelEngine::CreateApplication() {
	return new MinecraftClone();
}
