#include <VoxelEngine.h>
#include <VoxelEngine/Core/EntryPoint.h>
#include "Layers/GameLayer.h"
//#include "SimpleExamples/SimpleExample8.h"
#include "OpenGLSuperbible/Part1.h"
class MinecraftClone : public VoxelEngine::Application {
public:
	MinecraftClone() {
		//PushLayer<GameLayer>();
		//PushLayer<SimpleExample8>();
		PushLayer<Part1>();
	}
	~MinecraftClone() {

	}
};
VoxelEngine::Application* VoxelEngine::CreateApplication() {
	return new MinecraftClone();
}
