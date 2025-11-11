#include "GameLayer.h"
#include "../Overlays/DebugOverlay.h"
#include <glm/gtc/matrix_transform.hpp>

struct FaceData {
  uint32_t packedPos;
};

struct FaceModel {
  const glm::vec2 *texCoordsOrigin;
};
const int CHUNK_WIDTH = 16;
const int WORLD_WIDTH = 65;
const int WORLD_HEIGHT = 16;
const int TOTAL_CHUNKS = WORLD_WIDTH * WORLD_WIDTH * WORLD_HEIGHT;
const int BLOCKS_IN_CHUNK_COUNT = CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_WIDTH;
const int FACES_PER_CHUNK = BLOCKS_IN_CHUNK_COUNT;
const int TNT_WIDTH = 100;
const int TNT_COUNT = TNT_WIDTH * TNT_WIDTH * TNT_WIDTH;
const glm::vec3 DEFAULT_SPAWN(CHUNK_WIDTH *WORLD_WIDTH / 2,
                              CHUNK_WIDTH *WORLD_HEIGHT,
                              CHUNK_WIDTH *WORLD_WIDTH / 2);
const uint32_t HALF_WORLD_WIDTH = std::ceil(WORLD_WIDTH / 2.0f);
const uint32_t HALF_WORLD_HEIGHT = std::ceil(WORLD_HEIGHT / 2.0f);
const int DEBUG_LINES_FLOAT_COUNT = WORLD_HEIGHT * 16 * 16 * 3 + 2 * 8 * 4 * 3;

const int VERTS_PER_QUAD = 6;
const int QUADS_PER_BLOCK = 6;
const int VERTS_PER_CHUNK =
    BLOCKS_IN_CHUNK_COUNT * QUADS_PER_BLOCK * VERTS_PER_QUAD;
const std::string GLOBAL_SHADER_DEFINES = R"( 
#version 460 core
#define CHUNK_WIDTH )" + std::to_string(CHUNK_WIDTH) +
                                          R"(
#define TNT_WIDTH )" + std::to_string(TNT_WIDTH) +
                                          R"(
#define TNT_COUNT )" + std::to_string(TNT_COUNT) +
                                          R"(
#define WORLD_WIDTH )" + std::to_string(WORLD_WIDTH) +
                                          R"(
#define WORLD_HEIGHT )" + std::to_string(WORLD_HEIGHT) +
                                          R"(
#define BLOCKS_IN_CHUNK_COUNT )" + std::to_string(BLOCKS_IN_CHUNK_COUNT) +
                                          R"( 
#define FACES_PER_CHUNK )" + std::to_string(FACES_PER_CHUNK) +
                                          R"( 
#define HALF_WORLD_WIDTH )" + std::to_string(HALF_WORLD_WIDTH) +
                                          R"( 
#define HALF_WORLD_HEIGHT )" + std::to_string(HALF_WORLD_HEIGHT) +
                                          R"( 
#define DEFAULT_SPAWN vec3(CHUNK_WIDTH * WORLD_WIDTH / 2, CHUNK_WIDTH * WORLD_HEIGHT*2, CHUNK_WIDTH * WORLD_WIDTH / 2)
#define GRAVITY -28.57
#define TNT_EXPLOSION_STRENGTH 4

struct Chunk {
	uint x;
	uint y;
	uint z;
	bool hasExplosion;
	uint blockTypes[CHUNK_WIDTH][CHUNK_WIDTH][CHUNK_WIDTH];
	uint explosions[CHUNK_WIDTH][CHUNK_WIDTH][CHUNK_WIDTH];
};

struct ChunkQuads {
	uint blockQuads[FACES_PER_CHUNK];
};

struct TntEntity{
	vec3 position;
	vec3 velocity;
	float secondsUntilExplode;
	bool visible; 
};

#define air 0
#define dirt_block 1
#define grass_block 2
#define tnt_block 3
#define bedrock_block 4

#define top 0
#define bottom 1
#define east 2
#define west 3
#define south 4
#define north 5


#define dirt 0
#define grass_block_top 1
#define grass_block_side 2
#define stone 3
#define tnt_bottom 4
#define tnt_side 5
#define tnt_top 6
#define bedrock 7

#define MASK_3_BITS  0x7u
#define MASK_4_BITS  0xFu
#define MASK_7_BITS  0x7Fu
#define MASK_8_BITS  0xFFu
#define MASK_24_BITS 0xFFFFFFu

uint getChunkIndex(uint chunkX, uint chunkY, uint chunkZ){
	return chunkX+chunkY*WORLD_WIDTH+chunkZ*WORLD_WIDTH*WORLD_HEIGHT;
}
vec3 hash33(vec3 p)
{
    p = fract(p * 0.1031);
    p += dot(p, p.yzx + 33.33);
    return fract((p.xxy + p.yzz) * p.zyx);
}
)";

struct Chunk {
  int x;
  int y;
  int z;
  int hasExplosion;
  uint32_t blockTypes[CHUNK_WIDTH][CHUNK_WIDTH][CHUNK_WIDTH];
  uint32_t explosions[CHUNK_WIDTH][CHUNK_WIDTH][CHUNK_WIDTH];
};

struct ChunkQuads {
  uint32_t blockQuads[FACES_PER_CHUNK];
};

GameLayer::GameLayer()
    : Layer("Example"), m_Camera(70.0f, 0.1f, 1500.0f),
      m_SquarePosition(-1.1f, 0, -0.5f), m_CameraPosition(DEFAULT_SPAWN) {
  VE_PROFILE_FUNCTION;
  m_CameraPosition.y -= 150;
  // m_CameraPosition.x -= 300;
  // InitDebugLines();

  m_ShaderLibrary.Load("assets/shaders/lines.glsl", GLOBAL_SHADER_DEFINES);
  {
    VE_PROFILE_SCOPE("Bake texture atlas");
    m_TerrainAtlas = VoxelEngine::TextureAtlas::Create();
    VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> dirt =
        VoxelEngine::TextureAtlas::CreateTextureSubImage(
            "assets/textures/texture_pack/assets/minecraft/textures/block/"
            "dirt.png");
    VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> grass_block_top =
        VoxelEngine::TextureAtlas::CreateTextureSubImage(
            "assets/textures/texture_pack/assets/minecraft/textures/block/"
            "grass_block_top.png");
    grass_block_top->Colorize(m_GrassColorOverlay);
    VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> grass_block_side =
        VoxelEngine::TextureAtlas::CreateTextureSubImage(
            "assets/textures/texture_pack/assets/minecraft/textures/block/"
            "grass_block_side.png");
    VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> grass_block_side_overlay =
        VoxelEngine::TextureAtlas::CreateTextureSubImage(
            "assets/textures/texture_pack/assets/minecraft/textures/block/"
            "grass_block_side_overlay.png");
    grass_block_side_overlay->Colorize(m_GrassColorOverlay);
    grass_block_side->Combine(grass_block_side_overlay);
    VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> tnt_side =
        VoxelEngine::TextureAtlas::CreateTextureSubImage(
            "assets/textures/texture_pack/assets/minecraft/textures/block/"
            "tnt_side.png");
    VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> tnt_bottom =
        VoxelEngine::TextureAtlas::CreateTextureSubImage(
            "assets/textures/texture_pack/assets/minecraft/textures/block/"
            "tnt_bottom.png");
    VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> tnt_top =
        VoxelEngine::TextureAtlas::CreateTextureSubImage(
            "assets/textures/texture_pack/assets/minecraft/textures/block/"
            "tnt_top.png");
    VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> stone =
        VoxelEngine::TextureAtlas::CreateTextureSubImage(
            "assets/textures/texture_pack/assets/minecraft/textures/block/"
            "stone.png");
    VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> bedrock =
        VoxelEngine::TextureAtlas::CreateTextureSubImage(
            "assets/textures/texture_pack/assets/minecraft/textures/block/"
            "bedrock.png");
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

  uint32_t tntExplosionsQeueusSsbo;
  int queueCap = 63;
  {
    VE_PROFILE_SCOPE("Init tntExplosionsQeueusSsbo ssbo");
    glCreateBuffers(1, &tntExplosionsQeueusSsbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, tntExplosionsQeueusSsbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
                 HALF_WORLD_WIDTH * HALF_WORLD_WIDTH * HALF_WORLD_HEIGHT *
                     queueCap * 4 * sizeof(uint32_t),
                 nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, tntExplosionsQeueusSsbo);
  }
  uint32_t chunksSsbo;
  {
    VE_PROFILE_SCOPE("Init chunks data ssbo");
    glCreateBuffers(1, &chunksSsbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, chunksSsbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, TOTAL_CHUNKS * sizeof(Chunk),
                 nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, chunksSsbo);
  }
  uint32_t renderDataSsbo;
  {
    VE_PROFILE_SCOPE("Init renderDataSsbo ssbo");
    glCreateBuffers(1, &renderDataSsbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderDataSsbo);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, TOTAL_CHUNKS * sizeof(uint32_t),
                    nullptr,
                    GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT |
                        GL_MAP_COHERENT_BIT | GL_DYNAMIC_STORAGE_BIT);
    m_ChunksQuadCount = static_cast<uint32_t *>(glMapBufferRange(
        GL_SHADER_STORAGE_BUFFER, 0, TOTAL_CHUNKS * sizeof(uint32_t),
        GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT));
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, renderDataSsbo);
  }

  uint32_t hasWorldRedrawnSsbo;
  {
    VE_PROFILE_SCOPE("Init hasWorldRedrawnSsbo ssbo");
    glCreateBuffers(1, &hasWorldRedrawnSsbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, hasWorldRedrawnSsbo);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(uint32_t), nullptr,
                    GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT |
                        GL_MAP_COHERENT_BIT | GL_DYNAMIC_STORAGE_BIT);
    m_ShouldRedrawWorld = static_cast<bool *>(glMapBufferRange(
        GL_SHADER_STORAGE_BUFFER, 0, sizeof(uint32_t),
        GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT));
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, hasWorldRedrawnSsbo);
  }
  uint32_t hasChunkRedrawnSsbo;
  {
    VE_PROFILE_SCOPE("Init hasChunkRedrawnSsbo ssbo");
    glCreateBuffers(1, &hasChunkRedrawnSsbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, hasChunkRedrawnSsbo);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, TOTAL_CHUNKS * sizeof(uint32_t),
                    nullptr,
                    GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT |
                        GL_MAP_COHERENT_BIT | GL_DYNAMIC_STORAGE_BIT);
    m_ShouldRedrawChunk = static_cast<uint32_t *>(glMapBufferRange(
        GL_SHADER_STORAGE_BUFFER, 0, TOTAL_CHUNKS * sizeof(uint32_t),
        GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT));
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, hasChunkRedrawnSsbo);
  }
  {
    VE_PROFILE_SCOPE("Compute Shader: Generate blocks");
    // GEN BLOCKS
    auto generateBlocksCompute = m_ShaderLibrary.Load(
        "assets/shaders/compute/generateBlocks.glsl", GLOBAL_SHADER_DEFINES);
    generateBlocksCompute->Bind();
    glDispatchCompute(WORLD_WIDTH, WORLD_HEIGHT, WORLD_WIDTH);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT |
                    GL_BUFFER_UPDATE_BARRIER_BIT);
  }

  uint32_t genQuadsSsbo;
  {
    VE_PROFILE_SCOPE("Init quads for render ssbo");
    // GEN QUADS
    glCreateBuffers(1, &genQuadsSsbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, genQuadsSsbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, TOTAL_CHUNKS * sizeof(ChunkQuads),
                 nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, genQuadsSsbo);
  }

  m_ShaderLibrary.Load("assets/shaders/DrawTerrain.glsl",
                       GLOBAL_SHADER_DEFINES);
  m_ShaderLibrary.Load("assets/shaders/TntInstancing.glsl",
                       GLOBAL_SHADER_DEFINES);

  uint32_t quadInfo;
  glCreateBuffers(1, &quadInfo);
  const std::vector<glm::vec2> &subImagesCoordsList =
      m_TerrainAtlas->GetSubImagesCoordsList();
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, quadInfo);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
               subImagesCoordsList.size() * sizeof(glm::vec2),
               subImagesCoordsList.data(), GL_STATIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, quadInfo);

  float textureOffset = 16.0f / m_TerrainAtlas->GetWidth();
  uint32_t textureOffsets;
  glCreateBuffers(1, &textureOffsets);
  glm::vec2 textureOffsetsData[] = {
      {0.0, 0.0},
      {textureOffset, 0.0},
      {textureOffset, textureOffset},
      {0.0, textureOffset},
  };
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, textureOffsets);
  glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(textureOffsetsData),
               textureOffsetsData, GL_STATIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, textureOffsets);

  {
    GLuint indirectBuffer;
    glGenBuffers(1, &indirectBuffer);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
    // glBufferData(GL_DRAW_INDIRECT_BUFFER, TOTAL_CHUNKS *
    // sizeof(DrawArraysIndirectCommand), m_Cmd.data(), GL_STATIC_DRAW);
    glBufferStorage(GL_DRAW_INDIRECT_BUFFER,
                    TOTAL_CHUNKS * sizeof(DrawArraysIndirectCommand), nullptr,
                    GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT |
                        GL_MAP_COHERENT_BIT | GL_DYNAMIC_STORAGE_BIT);
    m_Cmd = static_cast<DrawArraysIndirectCommand *>(glMapBufferRange(
        GL_DRAW_INDIRECT_BUFFER, 0,
        TOTAL_CHUNKS * sizeof(DrawArraysIndirectCommand),
        GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT));
  }

  uint32_t tntEntitiesSsbo;
  {
    VE_PROFILE_SCOPE("Init tntEntitiesSsbo ssbo");
    glCreateBuffers(1, &tntEntitiesSsbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, tntEntitiesSsbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, TNT_COUNT * 48, nullptr,
                 GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, tntEntitiesSsbo);
  }
  m_ShaderLibrary.Load("assets/shaders/compute/initTntTransforms.glsl",
                       GLOBAL_SHADER_DEFINES);
  // m_ShaderLibrary.Load("assets/shaders/compute/initTntTransforms.glsl",
  // GLOBAL_SHADER_DEFINES)->Bind(); glDispatchCompute(TNT_COUNT, 1, 1);
  // glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  {
    VE_PROFILE_SCOPE("Load updateTntTransforms.glsl");
    m_ShaderLibrary.Load("assets/shaders/compute/updateTntTransforms.glsl",
                         GLOBAL_SHADER_DEFINES);
  }
  {
    VE_PROFILE_SCOPE("Load propagateExplosions.glsl");
    m_ShaderLibrary.Load("assets/shaders/compute/propagateExplosions.glsl",
                         GLOBAL_SHADER_DEFINES);
  }
  {
    VE_PROFILE_SCOPE("Load generateQuads.glsl");
    m_ShaderLibrary.Load("assets/shaders/compute/generateQuads.glsl",
                         GLOBAL_SHADER_DEFINES);
  }
  m_ShaderLibrary.Load("assets/shaders/compute/clearExplosions.glsl",
                       GLOBAL_SHADER_DEFINES);
  m_ShaderLibrary.Load("assets/shaders/compute/activateTnt.glsl",
                       GLOBAL_SHADER_DEFINES);
  m_ShaderLibrary.Load("assets/shaders/compute/explodeTnts.glsl",
                       GLOBAL_SHADER_DEFINES);
}
GameLayer::~GameLayer() {}
void GameLayer::OnAttach() {
  VE_PROFILE_FUNCTION;
  VoxelEngine::Application &application = VoxelEngine::Application::Get();
  application.GetWindow().SetMaximized(true);
  application.GetWindow().SetCursorVisibility(false);
  application.PushLayer<DebugOverlay>();
}
void GameLayer::OnUpdate(VoxelEngine::Timestep ts) {
  VE_PROFILE_FUNCTION;

  {
    VE_PROFILE_SCOPE("Key pooling");
    if (VoxelEngine::Input::IsKeyPressed(VE_KEY_A)) {
      m_CameraPosition -=
          glm::normalize(m_Camera.GetRight() * glm::vec3(1, 0, 1)) *
          (m_CameraMoveSpeed * ts);
    } else if (VoxelEngine::Input::IsKeyPressed(VE_KEY_D)) {
      m_CameraPosition +=
          glm::normalize(m_Camera.GetRight() * glm::vec3(1, 0, 1)) *
          (m_CameraMoveSpeed * ts);
    }
    if (VoxelEngine::Input::IsKeyPressed(VE_KEY_W)) {
      m_CameraPosition +=
          glm::normalize(m_Camera.GetFront() * glm::vec3(1, 0, 1)) *
          (m_CameraMoveSpeed * ts);
    } else if (VoxelEngine::Input::IsKeyPressed(VE_KEY_S)) {
      m_CameraPosition -=
          glm::normalize(m_Camera.GetFront() * glm::vec3(1, 0, 1)) *
          (m_CameraMoveSpeed * ts);
    }

    if (VoxelEngine::Input::IsKeyPressed(VE_KEY_SPACE)) {
      m_CameraPosition +=
          glm::vec3(0.0f, 1.0f, 0.0f) * (m_CameraMoveSpeed * ts);
    } else if (VoxelEngine::Input::IsKeyPressed(VE_KEY_LEFT_SHIFT)) {
      m_CameraPosition -=
          glm::vec3(0.0f, 1.0f, 0.0f) * (m_CameraMoveSpeed * ts);
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
    propagateExplosions->UploadUniformFloat3("u_Offset", {0, 0, 0});
    glDispatchCompute(HALF_WORLD_WIDTH, HALF_WORLD_HEIGHT, HALF_WORLD_WIDTH);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT |
                    GL_BUFFER_UPDATE_BARRIER_BIT);
    propagateExplosions->UploadUniformFloat3("u_Offset", {1, 0, 0});
    glDispatchCompute(HALF_WORLD_WIDTH, HALF_WORLD_HEIGHT, HALF_WORLD_WIDTH);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT |
                    GL_BUFFER_UPDATE_BARRIER_BIT);
    propagateExplosions->UploadUniformFloat3("u_Offset", {0, 0, 1});
    glDispatchCompute(HALF_WORLD_WIDTH, HALF_WORLD_HEIGHT, HALF_WORLD_WIDTH);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT |
                    GL_BUFFER_UPDATE_BARRIER_BIT);
    propagateExplosions->UploadUniformFloat3("u_Offset", {1, 0, 1});
    glDispatchCompute(HALF_WORLD_WIDTH, HALF_WORLD_HEIGHT, HALF_WORLD_WIDTH);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT |
                    GL_BUFFER_UPDATE_BARRIER_BIT);
    propagateExplosions->UploadUniformFloat3("u_Offset", {0, 1, 0});
    glDispatchCompute(HALF_WORLD_WIDTH, HALF_WORLD_HEIGHT, HALF_WORLD_WIDTH);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT |
                    GL_BUFFER_UPDATE_BARRIER_BIT);
    propagateExplosions->UploadUniformFloat3("u_Offset", {1, 1, 0});
    glDispatchCompute(HALF_WORLD_WIDTH, HALF_WORLD_HEIGHT, HALF_WORLD_WIDTH);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT |
                    GL_BUFFER_UPDATE_BARRIER_BIT);
    propagateExplosions->UploadUniformFloat3("u_Offset", {0, 1, 1});
    glDispatchCompute(HALF_WORLD_WIDTH, HALF_WORLD_HEIGHT, HALF_WORLD_WIDTH);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT |
                    GL_BUFFER_UPDATE_BARRIER_BIT);
    propagateExplosions->UploadUniformFloat3("u_Offset", {1, 1, 1});
    glDispatchCompute(HALF_WORLD_WIDTH, HALF_WORLD_HEIGHT, HALF_WORLD_WIDTH);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT |
                    GL_BUFFER_UPDATE_BARRIER_BIT);
  }

  {
    VE_PROFILE_SCOPE("Compute shader: update tnt transforms");
    auto updateTntTransformsCompute =
        m_ShaderLibrary.Get("updateTntTransforms");
    updateTntTransformsCompute->Bind();
    updateTntTransformsCompute->UploadUniformFloat("u_DeltaTime", ts);
    glDispatchCompute(ceil(TNT_COUNT / 256.0f), 1, 1);
    // glDispatchCompute(WORLD_WIDTH, WORLD_HEIGHT, WORLD_WIDTH);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
  }
  m_ShaderLibrary.Get("clearExplosions")->Bind();
  glDispatchCompute(WORLD_WIDTH, WORLD_HEIGHT, WORLD_WIDTH);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);

  {
    VE_PROFILE_SCOPE("Compute Shader: Generate quads");
    m_ShaderLibrary.Get("generateQuads")->Bind();
    glDispatchCompute(WORLD_WIDTH, WORLD_HEIGHT, WORLD_WIDTH);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT |
                    GL_BUFFER_UPDATE_BARRIER_BIT);
    // After compute shader or rendering that writes to SSBO:
    GLsync sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    // Wait until GPU finishes writing:
    // glClientWaitSync(sync, GL_SYNC_FLUSH_COMMANDS_BIT, 1000000);
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
  {
    VE_PROFILE_SCOPE("Draw");
    VoxelEngine::RenderCommand::SetClearColor({0.47059f, 0.6549f, 1.00f, 1});
    VoxelEngine::RenderCommand::Clear();
    VoxelEngine::Renderer::BeginScene(m_Camera);

    m_Camera.SetPosition(m_CameraPosition);

    auto drawTerrainShader = m_ShaderLibrary.Get("DrawTerrain");
    m_TerrainAtlas->Bind();
    VoxelEngine::Renderer::Submit(
        drawTerrainShader, glm::translate(glm::mat4(1), glm::vec3(0, 0, 0)));
    {
      VE_PROFILE_SCOPE("MultiDrawArraysIndirect");
      glMultiDrawArraysIndirect(GL_TRIANGLES, 0, TOTAL_CHUNKS, 0);
    }

    VoxelEngine::Renderer::Submit(
        m_ShaderLibrary.Get("TntInstancing"),
        glm::translate(glm::mat4(1), glm::vec3(0, 0, 0)));
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6 * 6, TNT_COUNT);

    // auto linesShader = m_ShaderLibrary.Get("lines");

    // UpdateDebugLines();
    // VoxelEngine::Renderer::Submit(linesShader,
    //	glm::translate(glm::mat4(1), glm::vec3(0, 0, 0))
    //);
    // m_LinesVA->Bind();
    // glDrawArrays(GL_LINES, 0, DEBUG_LINES_FLOAT_COUNT);
    VoxelEngine::Renderer::EndScene();
  }
}
void GameLayer::OnTick(VoxelEngine::Timestep ts) { VE_PROFILE_FUNCTION; }
void GameLayer::OnEvent(VoxelEngine::Event &event) {
  VoxelEngine::EventDispatcher dispatcher(event);
  dispatcher.Dispatch<VoxelEngine::MouseButtonReleasedEvent>(
      [&](VoxelEngine::MouseButtonReleasedEvent &e) {
        if (e.GetMouseButton() == VE_MOUSE_BUTTON_LEFT) {
          ActivateTnt();
        }
        return true;
      });
  dispatcher.Dispatch<VoxelEngine::KeyPressedEvent>(
      [&](VoxelEngine::KeyPressedEvent &e) {
        if (e.GetKeyCode() == VE_KEY_ESCAPE) {
          VoxelEngine::Application::Get().Close();
          return true;
        }
        if (e.GetKeyCode() == VE_KEY_C) {
          ForceRedraw();
          return true;
        }
        if (e.GetKeyCode() == VE_KEY_X) {
          SpawnTnts();
          return true;
        }
        return false;
      });
  dispatcher.Dispatch<VoxelEngine::MouseMovedEvent>(
      [&](VoxelEngine::MouseMovedEvent &e) {
        m_Camera.AddToYawAndPitch(e.GetXOffset(), e.GetYOffset());
        return true;
      });
  dispatcher.Dispatch<VoxelEngine::WindowResizeEvent>(
      [&](VoxelEngine::WindowResizeEvent &e) {
        m_Camera.RecalculateProjectionMatrix();
        return false;
      });
}

void GameLayer::ForceRedraw() {
  *m_ShouldRedrawWorld = true;
  for (int i = 0; i < TOTAL_CHUNKS; i++) {
    m_ShouldRedrawChunk[i] = true;
  }
}

void GameLayer::ActivateTnt() {
  auto activateTnt = m_ShaderLibrary.Get("activateTnt");
  activateTnt->Bind();
  activateTnt->UploadUniformFloat3("u_CameraPos", m_CameraPosition);
  activateTnt->UploadUniformFloat3("u_RayDirection", m_Camera.GetFront());
  glDispatchCompute(1, 1, 1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
}

void GameLayer::SpawnTnts() {
  m_ShaderLibrary.Get("initTntTransforms")->Bind();
  glDispatchCompute(TNT_COUNT, 1, 1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void GameLayer::InitDebugLines() {
  m_DebugLines.reserve(DEBUG_LINES_FLOAT_COUNT);
  m_LinesVA.reset(VoxelEngine::VertexArray::Create());
  VoxelEngine::Ref<VoxelEngine::VertexBuffer> linesVB(
      VoxelEngine::VertexBuffer::Create(nullptr, DEBUG_LINES_FLOAT_COUNT *
                                                     sizeof(float)));
  VoxelEngine::BufferLayout layout = {
      {VoxelEngine::ShaderDataType::Float3, "a_Position"},
  };
  linesVB->SetLayout(layout);
  m_LinesVA->AddVertexBuffer(linesVB);
  glLineWidth(1.0f);
}

void GameLayer::UpdateDebugLines() {
  m_DebugLines.clear();
  float hx = CHUNK_WIDTH;      // full size X
  float hy = CHUNK_WIDTH / 16; // full size Y
  float hz = CHUNK_WIDTH;      // full size Z
  // m_CameraPosition
  int chunkX = std::floor(m_CameraPosition.x / CHUNK_WIDTH);
  int chunkZ = std::floor(m_CameraPosition.z / CHUNK_WIDTH);
  float worldHeightBlocks = WORLD_HEIGHT * CHUNK_WIDTH;

  for (int i = 0; i <= CHUNK_WIDTH; i += 2) {
    glm::vec3 linesOrigin = {chunkX * CHUNK_WIDTH + i, 0, chunkZ * CHUNK_WIDTH};

    float verticalLine[] = {
        linesOrigin.x,
        linesOrigin.y,
        linesOrigin.z,
        linesOrigin.x,
        linesOrigin.y + worldHeightBlocks,
        linesOrigin.z,

        linesOrigin.x,
        linesOrigin.y,
        linesOrigin.z + hz,
        linesOrigin.x,
        linesOrigin.y + worldHeightBlocks,
        linesOrigin.z + hz,
    };
    m_DebugLines.insert(m_DebugLines.end(), std::begin(verticalLine),
                        std::end(verticalLine));
  }

  for (int i = 0; i <= CHUNK_WIDTH; i += 2) {
    glm::vec3 linesOrigin = {chunkX * CHUNK_WIDTH, 0, i + chunkZ * CHUNK_WIDTH};

    float verticalLine[] = {
        linesOrigin.x,
        linesOrigin.y,
        linesOrigin.z,
        linesOrigin.x,
        linesOrigin.y + worldHeightBlocks,
        linesOrigin.z,

        linesOrigin.x + hx,
        linesOrigin.y,
        linesOrigin.z,
        linesOrigin.x + hx,
        linesOrigin.y + worldHeightBlocks,
        linesOrigin.z,
    };
    m_DebugLines.insert(m_DebugLines.end(), std::begin(verticalLine),
                        std::end(verticalLine));
  }

  for (int y = 0; y < WORLD_HEIGHT; y++) {
    for (int i = 0; i < CHUNK_WIDTH; i += 2) {
      glm::vec3 linesOrigin = {chunkX * CHUNK_WIDTH, i + y * CHUNK_WIDTH,
                               chunkZ * CHUNK_WIDTH};

      float cubeLines[] = {
          // BOTTOM 4 edges
          linesOrigin.x,      linesOrigin.y, linesOrigin.z,
          linesOrigin.x + hx, linesOrigin.y, linesOrigin.z,

          linesOrigin.x + hx, linesOrigin.y, linesOrigin.z,
          linesOrigin.x + hx, linesOrigin.y, linesOrigin.z + hz,

          linesOrigin.x + hx, linesOrigin.y, linesOrigin.z + hz,
          linesOrigin.x,      linesOrigin.y, linesOrigin.z + hz,

          linesOrigin.x,      linesOrigin.y, linesOrigin.z + hz,
          linesOrigin.x,      linesOrigin.y, linesOrigin.z,
      };
      m_DebugLines.insert(m_DebugLines.end(), std::begin(cubeLines),
                          std::end(cubeLines));
    }
  }
  m_LinesVA->Bind();
  glBufferSubData(GL_ARRAY_BUFFER, 0, m_DebugLines.size() * sizeof(float),
                  m_DebugLines.data());
}

void GameLayer::OnImGuiRender() {
  // ImGui::Begin("Settings");
  // ImGui::End();
}

void GameLayer::OnDetach() {}
