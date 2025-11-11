#pragma once
#include <glad/glad.h>
#include <VoxelEngine.h>
struct DrawArraysIndirectCommand {
  GLuint count;         // number of vertices per draw
  GLuint instanceCount; // usually 1
  GLuint first;         // starting vertex
  GLuint baseInstance;  // optional (for per-chunk uniforms)
};
class GameLayer : public VoxelEngine::Layer {
public:
  GameLayer();
  ~GameLayer();
  void OnAttach() override;
  void OnUpdate(VoxelEngine::Timestep ts) override;
  void OnTick(VoxelEngine::Timestep ts) override;
  void OnEvent(VoxelEngine::Event &event) override;
  void ForceRedraw();
  void ActivateTnt();
  void SpawnTnts();
  void InitDebugLines();
  void UpdateDebugLines();
  virtual void OnImGuiRender() override;
  virtual void OnDetach() override;

  inline VoxelEngine::PerspectiveCamera &GetCamera() { return m_Camera; }

private:
  VoxelEngine::ShaderLibrary m_ShaderLibrary;
  VoxelEngine::Ref<VoxelEngine::VertexArray> m_SquareVA;
  VoxelEngine::Ref<VoxelEngine::Texture2D> m_Texture, m_ChernoLogoTexture;
  VoxelEngine::Ref<VoxelEngine::TextureAtlas> m_TerrainAtlas;
  VoxelEngine::PerspectiveCamera m_Camera;
  glm::vec3 m_SquarePosition;
  glm::vec3 m_CameraPosition;
  float m_CameraMoveSpeed = 50.0f;
  VoxelEngine::Ref<VoxelEngine::VertexArray> m_SsboVao;
  VoxelEngine::Ref<VoxelEngine::VertexArray> m_DirtBlock;
  DrawArraysIndirectCommand *m_Cmd;
  uint32_t *m_ChunksQuadCount;
  bool *m_ShouldRedrawWorld;
  uint32_t *m_ShouldRedrawChunk;
  const glm::vec3 m_GrassColorOverlay{112.0f, 160.0f, 70.0f};
  VoxelEngine::Ref<VoxelEngine::VertexArray> m_LinesVA;
  std::vector<float> m_DebugLines;
};
