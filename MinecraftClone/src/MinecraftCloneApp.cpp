#include <VoxelEngine.h>
#include <VoxelEngine/Core/EntryPoint.h>
#include "Layers/GameLayer.h"
//#include "SimpleExamples/SimpleExample7.h"
class MinecraftClone : public VoxelEngine::Application {
public:
	MinecraftClone() {
		PushLayer<GameLayer>();
		//PushLayer<SimpleExample7>();
	}
	~MinecraftClone() {

	}
};
VoxelEngine::Application* VoxelEngine::CreateApplication() {
	return new MinecraftClone();
}
