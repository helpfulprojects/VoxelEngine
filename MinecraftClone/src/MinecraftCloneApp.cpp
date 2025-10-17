#include <VoxelEngine.h>
#include <VoxelEngine/Core/EntryPoint.h>
#include "SimpleExamples/SimpleExample5.h"
class MinecraftClone : public VoxelEngine::Application {
public:
	MinecraftClone() {
		PushLayer<SimpleExample5>();
	}
	~MinecraftClone() {

	}
};
VoxelEngine::Application* VoxelEngine::CreateApplication() {
	return new MinecraftClone();
}
