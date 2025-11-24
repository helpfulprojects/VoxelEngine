#include "pch.h"
#include "Renderer.h"

namespace VoxelEngine {
Renderer::SceneData *Renderer::m_SceneData = new Renderer::SceneData;
void Renderer::BeginScene(PerspectiveCamera &camera) {
  m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
  m_SceneData->ViewMatrix = camera.GetViewMatrix();
  m_SceneData->ProjectionMatrix = camera.GetProjectionMatrix();
}
void Renderer::EndScene() {}
void Renderer::Init() {
  VE_PROFILE_FUNCTION;
  RenderCommand::Init();
}
void Renderer::Submit(const Ref<Shader> &shader,
                      const Ref<VertexArray> &vertexArray,
                      const glm::mat4 &transform) {
  shader->Bind();
  shader->UploadUniformMat4("u_ViewProjection",
                            m_SceneData->ViewProjectionMatrix);
  shader->UploadUniformMat4("u_Transform", transform);

  vertexArray->Bind();
  if (!vertexArray->GetVertexBuffers().empty()) {
    RenderCommand::DrawIndexed(vertexArray);
  }
}
void Renderer::Submit(const Ref<Shader> &shader, const glm::mat4 &transform) {
  shader->Bind();
  shader->UploadUniformMat4("u_ViewProjection",
                            m_SceneData->ViewProjectionMatrix);
  shader->UploadUniformMat4("u_View", m_SceneData->ViewMatrix);
  shader->UploadUniformMat4("u_Projection", m_SceneData->ProjectionMatrix);
  shader->UploadUniformMat4("u_Transform", transform);
}
} // namespace VoxelEngine