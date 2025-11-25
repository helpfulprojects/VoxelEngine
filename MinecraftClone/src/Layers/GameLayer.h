#pragma once
#include <glad/glad.h>
#include <VoxelEngine.h>
#include <raudio.h>
#define MAX_EXPLOSION_SOUNDS 10000
struct DrawArraysIndirectCommand {
    GLuint count;
    GLuint instanceCount;
    GLuint first;
    GLuint baseInstance;
};
class GameLayer : public VoxelEngine::Layer {
public:
    GameLayer();
    ~GameLayer();
    void OnAttach() override;
    void OnUpdate(VoxelEngine::Timestep ts) override;
    void OnRender() override;
    void OnEvent(VoxelEngine::Event& event) override;
    virtual void OnDetach() override;
    inline VoxelEngine::PerspectiveCamera& GetCamera() { return m_Camera; }

private:
    void SetupShaders();
    void SetupTextures();
    void SetupStorageBuffers();

private:
    VoxelEngine::ShaderLibrary m_ShaderLibrary;

    VoxelEngine::Ref<VoxelEngine::TextureAtlas> m_TerrainAtlas;
    VoxelEngine::PerspectiveCamera m_Camera;
    glm::vec3 m_CameraPosition;
    float m_CameraMoveSpeed = 500.0f;

    DrawArraysIndirectCommand* m_Cmd;

    uint32_t* m_ChunksQuadCount;
    bool* m_ShouldRedrawWorld;
    uint32_t* m_ShouldRedrawChunk;
    const glm::vec3 m_GrassColorOverlay { 112.0f, 160.0f, 70.0f };
    VoxelEngine::Ref<VoxelEngine::VertexArray> m_LinesVA;
    std::vector<float> m_DebugLines;
    VoxelEngine::Ref<VoxelEngine::Texture2D> m_TntTexture;
    VoxelEngine::Ref<VoxelEngine::TextureAtlas> m_TntAtlas;
    Sound m_FuseSound;
    Sound m_ExplosionSounds[MAX_EXPLOSION_SOUNDS] = { 0 };
    bool* m_ShouldPlayFuseAudio;
    bool* m_DoesCurrentFrameHaveExplosion;
    uint32_t m_CurrentExplosionSound = 0;
    bool m_UpdateTntPosition = true;

    VoxelEngine::Ref<VoxelEngine::StorageBuffer> m_TntExplosionsQueues;
    VoxelEngine::Ref<VoxelEngine::StorageBuffer> m_ChunksSsbo;
    VoxelEngine::Ref<VoxelEngine::StorageBuffer> m_RenderDataSsbo;
    VoxelEngine::Ref<VoxelEngine::StorageBuffer> m_HasWorldRedrawnSsbo;
    VoxelEngine::Ref<VoxelEngine::StorageBuffer> m_HasChunkRedrawnSsbo;
    VoxelEngine::Ref<VoxelEngine::StorageBuffer> m_HasTntFuseLitSsbo;
    VoxelEngine::Ref<VoxelEngine::StorageBuffer> m_DoesCurrentFrameHaveExplosionSsbo;
    VoxelEngine::Ref<VoxelEngine::StorageBuffer> m_QuadInfoSsbo;
    VoxelEngine::Ref<VoxelEngine::StorageBuffer> m_TextureOffsetSsbo;

    VoxelEngine::Ref<VoxelEngine::StorageBuffer> m_DrawIndirectBuffer;
    VoxelEngine::Ref<VoxelEngine::StorageBuffer> m_TntEntitiesSsbo;
    VoxelEngine::Ref<VoxelEngine::StorageBuffer> m_GenQuadsSsbo;
};
