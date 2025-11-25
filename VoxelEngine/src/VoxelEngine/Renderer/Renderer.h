#pragma once
#include "RenderCommand.h"
#include "VoxelEngine/Renderer/PerspectiveCamera.h"
#include "VoxelEngine/Renderer/Shader.h"
namespace VoxelEngine {

class Renderer {
public:
    static void Init();
    static void BeginScene(PerspectiveCamera& camera);
    static void EndScene();
    static void Submit(const Ref<Shader>& shader, const glm::mat4& transform);

private:
    struct SceneData {
        glm::mat4 ViewProjectionMatrix;
        glm::mat4 ViewMatrix;
        glm::mat4 ProjectionMatrix;
    };

    static SceneData* m_SceneData;
};
} // namespace VoxelEngine
