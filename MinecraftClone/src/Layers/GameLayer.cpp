#include "GameLayer.h"

#include <filesystem>
#include <glm/gtc/matrix_transform.hpp>

#include "../Overlays/DebugOverlay.h"
#include "GameConfig.h"

GameLayer::GameLayer()
    : Layer("GameLayer")
    , m_Camera(70.0f, 0.1f, 2500.0f)
    , m_CameraPosition(DEFAULT_SPAWN)
{
    VE_PROFILE_FUNCTION;
    m_CameraPosition.y -= 150;
    m_CameraPosition.x -= 300;
    SetupShaders();
    SetupTextures();
    SetupStorageBuffers();

    {
        VE_PROFILE_SCOPE("Compute Shader: Generate blocks");
        m_ShaderLibrary.Get("generateBlocks")->Bind();
        glDispatchCompute(WORLD_WIDTH, WORLD_HEIGHT, WORLD_WIDTH);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    InitAudioDevice();
    m_FuseSound = LoadSound("assets/audio/Fuse.ogg");
    m_ExplosionSounds[0] = LoadSound("assets/audio/Explosion1.ogg");
    m_ExplosionSounds[1] = LoadSound("assets/audio/Explosion2.ogg");
    m_ExplosionSounds[2] = LoadSound("assets/audio/Explosion3.ogg");
    m_ExplosionSounds[3] = LoadSound("assets/audio/Explosion4.ogg");
    for (int i = 4; i < MAX_EXPLOSION_SOUNDS; i++) {
        int randomIndex = rand() % 4;
        m_ExplosionSounds[i] = LoadSoundAlias(m_ExplosionSounds[randomIndex]);
    }
}
GameLayer::~GameLayer() { }
void GameLayer::OnAttach()
{
    VE_PROFILE_FUNCTION;
    VoxelEngine::Application& application = VoxelEngine::Application::Get();
    application.GetWindow().SetMaximized(true);
    application.GetWindow().SetCursorVisibility(false);
    application.PushLayer<DebugOverlay>();
}
void GameLayer::OnUpdate(VoxelEngine::Timestep ts)
{
    VE_PROFILE_FUNCTION;

    {
        VE_PROFILE_SCOPE("Key pooling");
        if (VoxelEngine::Input::IsKeyPressed(VE_KEY_A)) {
            m_CameraPosition -= glm::normalize(m_Camera.GetRight() * glm::vec3(1, 0, 1)) * (m_CameraMoveSpeed * ts);
        } else if (VoxelEngine::Input::IsKeyPressed(VE_KEY_D)) {
            m_CameraPosition += glm::normalize(m_Camera.GetRight() * glm::vec3(1, 0, 1)) * (m_CameraMoveSpeed * ts);
        }
        if (VoxelEngine::Input::IsKeyPressed(VE_KEY_W)) {
            m_CameraPosition += glm::normalize(m_Camera.GetFront() * glm::vec3(1, 0, 1)) * (m_CameraMoveSpeed * ts);
        } else if (VoxelEngine::Input::IsKeyPressed(VE_KEY_S)) {
            m_CameraPosition -= glm::normalize(m_Camera.GetFront() * glm::vec3(1, 0, 1)) * (m_CameraMoveSpeed * ts);
        }

        if (VoxelEngine::Input::IsKeyPressed(VE_KEY_SPACE)) {
            m_CameraPosition += glm::vec3(0.0f, 1.0f, 0.0f) * (m_CameraMoveSpeed * ts);
        } else if (VoxelEngine::Input::IsKeyPressed(VE_KEY_LEFT_SHIFT)) {
            m_CameraPosition -= glm::vec3(0.0f, 1.0f, 0.0f) * (m_CameraMoveSpeed * ts);
        }
    }

    {
        VE_PROFILE_SCOPE("Compute shader: update tnt transforms");
        auto updateTntTransformsCompute = m_ShaderLibrary.Get("explodeTnts");
        updateTntTransformsCompute->Bind();
        updateTntTransformsCompute->UploadUniformFloat("u_DeltaTime", ts);
        glDispatchCompute(ceil(TNT_COUNT / 256.0f), 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }
    auto propagateExplosions = m_ShaderLibrary.Get("propagateExplosions");
    propagateExplosions->Bind();
    {
        VE_PROFILE_SCOPE("Compute shader: propagate explosions");
        propagateExplosions->UploadUniformFloat3("u_Offset", { 0, 0, 0 });
        glDispatchCompute(HALF_WORLD_WIDTH, HALF_WORLD_HEIGHT,
            HALF_WORLD_WIDTH);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        propagateExplosions->UploadUniformFloat3("u_Offset", { 1, 0, 0 });
        glDispatchCompute(HALF_WORLD_WIDTH, HALF_WORLD_HEIGHT,
            HALF_WORLD_WIDTH);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        propagateExplosions->UploadUniformFloat3("u_Offset", { 0, 0, 1 });
        glDispatchCompute(HALF_WORLD_WIDTH, HALF_WORLD_HEIGHT,
            HALF_WORLD_WIDTH);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        propagateExplosions->UploadUniformFloat3("u_Offset", { 1, 0, 1 });
        glDispatchCompute(HALF_WORLD_WIDTH, HALF_WORLD_HEIGHT,
            HALF_WORLD_WIDTH);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        propagateExplosions->UploadUniformFloat3("u_Offset", { 0, 1, 0 });
        glDispatchCompute(HALF_WORLD_WIDTH, HALF_WORLD_HEIGHT,
            HALF_WORLD_WIDTH);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        propagateExplosions->UploadUniformFloat3("u_Offset", { 1, 1, 0 });
        glDispatchCompute(HALF_WORLD_WIDTH, HALF_WORLD_HEIGHT,
            HALF_WORLD_WIDTH);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        propagateExplosions->UploadUniformFloat3("u_Offset", { 0, 1, 1 });
        glDispatchCompute(HALF_WORLD_WIDTH, HALF_WORLD_HEIGHT,
            HALF_WORLD_WIDTH);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        propagateExplosions->UploadUniformFloat3("u_Offset", { 1, 1, 1 });
        glDispatchCompute(HALF_WORLD_WIDTH, HALF_WORLD_HEIGHT,
            HALF_WORLD_WIDTH);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        GLsync sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        float secondsToWait = 1.0f;
        glClientWaitSync(sync, 0, secondsToWait * 1000000000);
        glDeleteSync(sync);
    }

    if (*m_DoesCurrentFrameHaveExplosion) {
        if (!IsSoundPlaying(m_ExplosionSounds[m_CurrentExplosionSound])) {
            PlaySound(m_ExplosionSounds[m_CurrentExplosionSound]);
        }
        m_CurrentExplosionSound++;
        if (m_CurrentExplosionSound >= MAX_EXPLOSION_SOUNDS) {
            m_CurrentExplosionSound = 0;
        }
        *m_DoesCurrentFrameHaveExplosion = false;
    }

    if (m_UpdateTntPosition) {
        {
            VE_PROFILE_SCOPE("Compute shader: update tnt transforms");
            auto updateTntTransformsCompute = m_ShaderLibrary.Get("updateTntTransforms");
            updateTntTransformsCompute->Bind();
            updateTntTransformsCompute->UploadUniformFloat("u_DeltaTime", ts);
            glDispatchCompute(ceil(TNT_COUNT / 256.0f), 1, 1);
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        }
    }
    m_ShaderLibrary.Get("clearExplosions")->Bind();
    glDispatchCompute(WORLD_WIDTH, WORLD_HEIGHT, WORLD_WIDTH);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    {
        VE_PROFILE_SCOPE("Compute Shader: Generate quads");
        m_ShaderLibrary.Get("generateQuads")->Bind();
        glDispatchCompute(WORLD_WIDTH, WORLD_HEIGHT, WORLD_WIDTH);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        GLsync sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        float secondsToWait = 1.0f;
        glClientWaitSync(sync, 0, secondsToWait * 1000000000);
        glDeleteSync(sync);
    }

    if (*m_ShouldRedrawWorld) {
        VE_PROFILE_SCOPE("Set up indirect buffer");
        if (m_ChunksQuadCount) {
            for (int i = 0; i < TOTAL_CHUNKS; i++) {
                if (m_ShouldRedrawChunk[i]) {
                    int count = m_ChunksQuadCount[i];
                    m_Cmd[i].count = count * VERTS_PER_QUAD;
                    m_Cmd[i].instanceCount = 1;
                    m_Cmd[i].first = 0;
                    m_Cmd[i].baseInstance = i;
                    m_ShouldRedrawChunk[i] = false;
                }
            }
        }
        *m_ShouldRedrawWorld = false;
    }
}
void GameLayer::OnRender()
{
    {
        VE_PROFILE_SCOPE("Draw");
        VoxelEngine::RenderCommand::SetClearColor(
            { 0.47059f, 0.6549f, 1.00f, 1 });
        VoxelEngine::RenderCommand::Clear();
        VoxelEngine::Renderer::BeginScene(m_Camera);

        m_Camera.SetPosition(m_CameraPosition);

        auto drawTerrainShader = m_ShaderLibrary.Get("drawTerrain");
        m_TerrainAtlas->Bind();
        VoxelEngine::Renderer::Submit(
            drawTerrainShader,
            glm::translate(glm::mat4(1), glm::vec3(0, 0, 0)));
        glMultiDrawArraysIndirect(GL_TRIANGLES, 0, TOTAL_CHUNKS, 0);

        VoxelEngine::Renderer::Submit(
            m_ShaderLibrary.Get("tntInstancing"),
            glm::translate(glm::mat4(1), glm::vec3(0, 0, 0)));
        glDrawArrays(GL_POINTS, 0, TNT_COUNT);
        VoxelEngine::Renderer::EndScene();
    }
}
void GameLayer::OnEvent(VoxelEngine::Event& event)
{
    VoxelEngine::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<VoxelEngine::MouseButtonReleasedEvent>(
        [&](VoxelEngine::MouseButtonReleasedEvent& e) {
            if (e.GetMouseButton() == VE_MOUSE_BUTTON_LEFT) {
                auto activateTnt = m_ShaderLibrary.Get("activateTnt");
                activateTnt->Bind();
                activateTnt->UploadUniformFloat3("u_CameraPos",
                    m_CameraPosition);
                activateTnt->UploadUniformFloat3("u_RayDirection",
                    m_Camera.GetFront());
                glDispatchCompute(1, 1, 1);
                glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
                GLsync sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
                float secondsToWait = 1.0f;
                glClientWaitSync(sync, 0, secondsToWait * 1000000000);
                glDeleteSync(sync);
                if (*m_ShouldPlayFuseAudio) {
                    PlaySound(m_FuseSound);
                    *m_ShouldPlayFuseAudio = false;
                }
            }
            return true;
        });
    dispatcher.Dispatch<VoxelEngine::KeyPressedEvent>(
        [&](VoxelEngine::KeyPressedEvent& e) {
            if (e.GetKeyCode() == VE_KEY_ESCAPE) {
                VoxelEngine::Application::Get().Close();
                return true;
            }
            if (e.GetKeyCode() == VE_KEY_1) {
                m_CameraMoveSpeed = 5;
                return true;
            }
            if (e.GetKeyCode() == VE_KEY_2) {
                m_CameraMoveSpeed = 50;
                return true;
            }
            if (e.GetKeyCode() == VE_KEY_3) {
                m_CameraMoveSpeed = 500;
                return true;
            }
            return false;
        });
    dispatcher.Dispatch<VoxelEngine::MouseMovedEvent>(
        [&](VoxelEngine::MouseMovedEvent& e) {
            m_Camera.AddToYawAndPitch(e.GetXOffset(), e.GetYOffset());
            return true;
        });
    dispatcher.Dispatch<VoxelEngine::WindowResizeEvent>(
        [&](VoxelEngine::WindowResizeEvent& e) {
            m_Camera.RecalculateProjectionMatrix();
            return false;
        });
}

void GameLayer::OnDetach() { CloseAudioDevice(); }

void GameLayer::SetupShaders()
{
    const std::string SHADERS_GLOBAL_INCLUDE_SOURCE = R"( 
						#define CHUNK_SIDE_LENGTH )"
        + std::to_string(CHUNK_SIDE_LENGTH) +
        R"(
						#define TNT_SIDE_LENGTH )"
        + std::to_string(TNT_SIDE_LENGTH) +
        R"(
						#define TNT_COUNT )"
        + std::to_string(TNT_COUNT) +
        R"(
						#define TOTAL_CHUNKS )"
        + std::to_string(TOTAL_CHUNKS) +
        R"(
						#define WORLD_WIDTH )"
        + std::to_string(WORLD_WIDTH) +
        R"(
						#define WORLD_HEIGHT )"
        + std::to_string(WORLD_HEIGHT) +
        R"(
						#define BLOCKS_IN_CHUNK_COUNT )"
        + std::to_string(BLOCKS_IN_CHUNK_COUNT) +
        R"( 
						#define FACES_PER_CHUNK )"
        + std::to_string(FACES_PER_CHUNK) +
        R"( 
						#define HALF_WORLD_WIDTH )"
        + std::to_string(HALF_WORLD_WIDTH) +
        R"( 
						#define HALF_WORLD_HEIGHT )"
        + std::to_string(HALF_WORLD_HEIGHT) +
        R"( 
						#define SURFACE_LEVEL )"
        + std::to_string(SURFACE_LEVEL) + "\n";
    m_ShaderLibrary.SetGlobalIncludeSource(SHADERS_GLOBAL_INCLUDE_SOURCE);
    m_ShaderLibrary.SetGlobalIncludeFile("assets/shaders/globalInclude.glsl");

    m_ShaderLibrary.Load("assets/shaders/compute/generateBlocks.glsl");
    m_ShaderLibrary.Load("assets/shaders/drawTerrain.glsl");
    m_ShaderLibrary.Load("assets/shaders/tntInstancing.glsl");
    m_ShaderLibrary.Load("assets/shaders/compute/initTntTransforms.glsl");
    m_ShaderLibrary.Load("assets/shaders/compute/updateTntTransforms.glsl");
    m_ShaderLibrary.Load("assets/shaders/compute/propagateExplosions.glsl");
    m_ShaderLibrary.Load("assets/shaders/compute/generateQuads.glsl");
    m_ShaderLibrary.Load("assets/shaders/compute/clearExplosions.glsl");
    m_ShaderLibrary.Load("assets/shaders/compute/activateTnt.glsl");
    m_ShaderLibrary.Load("assets/shaders/compute/explodeTnts.glsl");
}

void GameLayer::SetupTextures()
{
    std::string blocksFolderLocation = "assets/textures/texture_pack/assets/minecraft/textures/block/";
    std::string blocksFolderLocationAlternative = "assets/textures/texture_pack/assets/minecraft/textures/blocks/";
    std::string selectedFolder;
    if (std::filesystem::exists(blocksFolderLocation)) {
        selectedFolder = blocksFolderLocation;
    } else if (std::filesystem::exists(blocksFolderLocationAlternative)) {
        selectedFolder = blocksFolderLocationAlternative;
    } else {
        VE_ERROR("No valid block texture folder found!");
    }
    std::string grassBlockPrefix = "grass_block";
    if (!std::filesystem::exists(selectedFolder + grassBlockPrefix + "_top.png")) {
        grassBlockPrefix = "grass";
    }
    {
        VE_PROFILE_SCOPE("Bake tnt atlas");
        m_TntAtlas = VoxelEngine::TextureAtlas::Create();
        VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> tnt_side00 = VoxelEngine::TextureAtlas::CreateTextureSubImage(selectedFolder + "tnt_side.png");
        tnt_side00->SetOffset(0, 0);
        tnt_side00->Rotate(180);
        VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> tnt_bottom01 = VoxelEngine::TextureAtlas::CreateTextureSubImage(selectedFolder + "tnt_bottom.png");
        tnt_bottom01->SetOffset(0, 1);
        VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> tnt_side11 = VoxelEngine::TextureAtlas::CreateTextureSubImage(selectedFolder + "tnt_side.png");
        tnt_side11->SetOffset(1, 1);
        tnt_side11->Rotate(270);
        VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> tnt_top21 = VoxelEngine::TextureAtlas::CreateTextureSubImage(selectedFolder + "tnt_top.png");
        tnt_top21->SetOffset(2, 1);
        VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> tnt_side31 = VoxelEngine::TextureAtlas::CreateTextureSubImage(selectedFolder + "tnt_side.png");
        tnt_side31->SetOffset(3, 1);
        tnt_side31->Rotate(90);
        VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> tnt_bottom41 = VoxelEngine::TextureAtlas::CreateTextureSubImage(selectedFolder + "tnt_bottom.png");
        tnt_bottom41->SetOffset(4, 1);
        VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> tnt_side42 = VoxelEngine::TextureAtlas::CreateTextureSubImage(selectedFolder + "tnt_side.png");
        tnt_side42->SetOffset(4, 2);
        m_TntAtlas->Add(tnt_side00);
        m_TntAtlas->Add(tnt_bottom01);
        m_TntAtlas->Add(tnt_side11);
        m_TntAtlas->Add(tnt_top21);
        m_TntAtlas->Add(tnt_side31);
        m_TntAtlas->Add(tnt_bottom41);
        m_TntAtlas->Add(tnt_side42);
        m_TntAtlas->Bake(8);
        m_TntAtlas->Bind(1);
    }
    // TERRAIN ATLAS
    {
        VE_PROFILE_SCOPE("Bake texture atlas");
        m_TerrainAtlas = VoxelEngine::TextureAtlas::Create();
        VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> dirt = VoxelEngine::TextureAtlas::CreateTextureSubImage(selectedFolder + "dirt.png");
        VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> grass_block_top = VoxelEngine::TextureAtlas::CreateTextureSubImage(
            selectedFolder + grassBlockPrefix + "_top.png");
        grass_block_top->ToRGBA();
        grass_block_top->Colorize(m_GrassColorOverlay);
        VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> grass_block_side = VoxelEngine::TextureAtlas::CreateTextureSubImage(
            selectedFolder + grassBlockPrefix + "_side.png");
        VoxelEngine::Ref<VoxelEngine::TextureSubImage2D>
            grass_block_side_overlay = VoxelEngine::TextureAtlas::CreateTextureSubImage(
                selectedFolder + grassBlockPrefix + "_side_overlay.png");
        grass_block_side_overlay->ToRGBA();
        grass_block_side_overlay->Colorize(m_GrassColorOverlay);
        grass_block_side->Combine(grass_block_side_overlay);
        VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> tnt_side = VoxelEngine::TextureAtlas::CreateTextureSubImage(selectedFolder + "tnt_side.png");
        VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> tnt_bottom = VoxelEngine::TextureAtlas::CreateTextureSubImage(selectedFolder + "tnt_bottom.png");
        VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> tnt_top = VoxelEngine::TextureAtlas::CreateTextureSubImage(selectedFolder + "tnt_top.png");
        VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> stone = VoxelEngine::TextureAtlas::CreateTextureSubImage(selectedFolder + "stone.png");
        stone->ToRGBA();
        VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> bedrock = VoxelEngine::TextureAtlas::CreateTextureSubImage(selectedFolder + "bedrock.png");
        bedrock->ToRGBA();
        m_TerrainAtlas->Add(dirt);
        m_TerrainAtlas->Add(grass_block_top);
        m_TerrainAtlas->Add(grass_block_side);
        m_TerrainAtlas->Add(stone);
        m_TerrainAtlas->Add(tnt_bottom);
        m_TerrainAtlas->Add(tnt_side);
        m_TerrainAtlas->Add(tnt_top);
        m_TerrainAtlas->Add(bedrock);
        m_TerrainAtlas->Bake();
    }
}

void GameLayer::SetupStorageBuffers()
{

    struct Chunk {
        int x;
        int y;
        int z;
        bool hasExplosion;
        bool _pad0[3];
        uint32_t blockTypes[CHUNK_SIDE_LENGTH][CHUNK_SIDE_LENGTH]
                           [CHUNK_SIDE_LENGTH];
        uint32_t explosions[CHUNK_SIDE_LENGTH][CHUNK_SIDE_LENGTH]
                           [CHUNK_SIDE_LENGTH];
    };
    struct TntEntity {
        glm::vec3 position;
        uint32_t _pad0;
        glm::vec3 velocity;
        uint32_t _pad1;
        float secondsUntilExplode;
        bool visible;
        bool _pad2[3];
        bool justBlewUp;
        bool _pad3[3];
        uint32_t _pad4; // had to set this last padding because struct size has to be a multiple of its largest alignment when using the standard layout supposed to the shared layout
    };

    struct ChunkQuads {
        uint32_t blockQuads[FACES_PER_CHUNK];
    };

    struct PropagationQueueNode {
        uint32_t localIndex3DPacked;
        uint32_t chunkIndex3DPacked;
        uint32_t chunkIndex;
        uint32_t previousValue;
    };
    uint32_t PERSISTENT_READ_BITMASK = GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
    uint32_t PERSISTENT_WRITE_BITMASK = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

    /////////////////
    /// Render
    /////////////////
    m_RenderDataSsbo = VoxelEngine::StorageBuffer::Create(GL_SHADER_STORAGE_BUFFER,
        nullptr, TOTAL_CHUNKS * sizeof(uint32_t),
        PERSISTENT_READ_BITMASK | GL_DYNAMIC_STORAGE_BIT);
    m_ChunksQuadCount = static_cast<uint32_t*>(m_RenderDataSsbo->MapBufferRange(
        0, TOTAL_CHUNKS * sizeof(uint32_t), PERSISTENT_READ_BITMASK));
    m_RenderDataSsbo->BindBufferBase(5);

    m_ShouldRedrawWorldSsbo = VoxelEngine::StorageBuffer::Create(GL_SHADER_STORAGE_BUFFER,
        nullptr, sizeof(uint32_t),
        PERSISTENT_WRITE_BITMASK | GL_DYNAMIC_STORAGE_BIT);
    m_ShouldRedrawWorld = static_cast<bool*>(m_ShouldRedrawWorldSsbo->MapBufferRange(
        0, sizeof(uint32_t), PERSISTENT_WRITE_BITMASK));
    m_ShouldRedrawWorldSsbo->BindBufferBase(8);

    m_ShouldRedrawChunkSsbo = VoxelEngine::StorageBuffer::Create(GL_SHADER_STORAGE_BUFFER,
        nullptr, TOTAL_CHUNKS * sizeof(uint32_t),
        PERSISTENT_WRITE_BITMASK | GL_DYNAMIC_STORAGE_BIT);
    m_ShouldRedrawChunk = static_cast<uint32_t*>(m_ShouldRedrawChunkSsbo->MapBufferRange(
        0, TOTAL_CHUNKS * sizeof(uint32_t), PERSISTENT_WRITE_BITMASK));
    m_ShouldRedrawChunkSsbo->BindBufferBase(9);

    m_GenQuadsSsbo = VoxelEngine::StorageBuffer::Create(GL_SHADER_STORAGE_BUFFER,
        nullptr,
        TOTAL_CHUNKS * sizeof(ChunkQuads),
        GL_DYNAMIC_STORAGE_BIT);
    m_GenQuadsSsbo->BindBufferBase(1);

    m_DrawIndirectBuffer = VoxelEngine::StorageBuffer::Create(GL_DRAW_INDIRECT_BUFFER,
        nullptr, TOTAL_CHUNKS * sizeof(DrawArraysIndirectCommand),
        PERSISTENT_WRITE_BITMASK | GL_DYNAMIC_STORAGE_BIT);
    m_Cmd = static_cast<DrawArraysIndirectCommand*>(m_DrawIndirectBuffer->MapBufferRange(
        0, TOTAL_CHUNKS * sizeof(DrawArraysIndirectCommand), PERSISTENT_WRITE_BITMASK));
    m_DrawIndirectBuffer->Bind();

    /////////////////
    /// Tnts
    /////////////////
    m_TntEntitiesSsbo = VoxelEngine::StorageBuffer::Create(GL_SHADER_STORAGE_BUFFER,
        nullptr, TNT_COUNT * sizeof(TntEntity), GL_DYNAMIC_STORAGE_BIT);
    m_TntEntitiesSsbo->BindBufferBase(6);

    int MAX_PROPAGATION_QUEUE_SIZE = 63;
    m_TntExplosionsQueues = VoxelEngine::StorageBuffer::Create(GL_SHADER_STORAGE_BUFFER,
        nullptr,
        HALF_WORLD_WIDTH * HALF_WORLD_WIDTH * HALF_WORLD_HEIGHT * MAX_PROPAGATION_QUEUE_SIZE * sizeof(PropagationQueueNode),
        GL_DYNAMIC_STORAGE_BIT);
    m_TntExplosionsQueues->BindBufferBase(7);

    m_ChunksSsbo = VoxelEngine::StorageBuffer::Create(GL_SHADER_STORAGE_BUFFER,
        nullptr, TOTAL_CHUNKS * sizeof(Chunk), GL_DYNAMIC_STORAGE_BIT);
    m_ChunksSsbo->BindBufferBase(0);

    /////////////////
    /// Audio
    /////////////////
    m_HasTntFuseLitSsbo = VoxelEngine::StorageBuffer::Create(GL_SHADER_STORAGE_BUFFER,
        nullptr, sizeof(uint32_t),
        PERSISTENT_WRITE_BITMASK | GL_DYNAMIC_STORAGE_BIT);
    m_ShouldPlayFuseAudio = static_cast<bool*>(m_HasTntFuseLitSsbo->MapBufferRange(
        0, sizeof(uint32_t), PERSISTENT_WRITE_BITMASK));
    m_HasTntFuseLitSsbo->BindBufferBase(10);

    m_DoesCurrentFrameHaveExplosionSsbo = VoxelEngine::StorageBuffer::Create(GL_SHADER_STORAGE_BUFFER,
        nullptr, sizeof(uint32_t),
        PERSISTENT_WRITE_BITMASK | GL_DYNAMIC_STORAGE_BIT);
    m_DoesCurrentFrameHaveExplosion = static_cast<bool*>(m_DoesCurrentFrameHaveExplosionSsbo->MapBufferRange(
        0, sizeof(uint32_t), PERSISTENT_WRITE_BITMASK));
    m_DoesCurrentFrameHaveExplosionSsbo->BindBufferBase(11);

    /////////////////
    /// Textures
    /////////////////
    const std::vector<glm::vec2>& subImagesCoordsList = m_TerrainAtlas->GetSubImagesCoordsList();
    m_QuadInfoSsbo = VoxelEngine::StorageBuffer::Create(GL_SHADER_STORAGE_BUFFER,
        subImagesCoordsList.data(), subImagesCoordsList.size() * sizeof(glm::vec2), 0);
    m_QuadInfoSsbo->BindBufferBase(2);

    float textureOffset = (float)m_TerrainAtlas->GetSpriteSize() / m_TerrainAtlas->GetWidth();
    glm::vec2 textureOffsetsData[] = {
        { 0.0, 0.0 },
        { textureOffset, 0.0 },
        { textureOffset, textureOffset },
        { 0.0, textureOffset },
    };
    m_TextureOffsetSsbo = VoxelEngine::StorageBuffer::Create(GL_SHADER_STORAGE_BUFFER,
        textureOffsetsData, sizeof(textureOffsetsData), 0);
    m_TextureOffsetSsbo->BindBufferBase(3);
}
