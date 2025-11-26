#include <VoxelEngine.h>
#include <VoxelEngine/Core/EntryPoint.h>
#include "Layers/GameLayer.h"
class MinecraftClone : public VoxelEngine::Application {
public:
    MinecraftClone() { PushLayer<GameLayer>(); }
    ~MinecraftClone() { }
};
VoxelEngine::Application* VoxelEngine::CreateApplication()
{
    return new MinecraftClone();
}
