#include <VoxelEngine.h>
class MinecraftClone : public VoxelEngine::Application {
public:
	MinecraftClone() {

	}
	~MinecraftClone() {

	}
};
VoxelEngine::Application* VoxelEngine::CreateApplication() {
	return new MinecraftClone();
}
