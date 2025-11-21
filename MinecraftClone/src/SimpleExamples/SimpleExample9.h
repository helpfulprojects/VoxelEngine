#pragma once
#include <VoxelEngine.h>
#include <glad/glad.h>
#include "../Overlays/DebugOverlay.h"
class SimpleExample9 : public VoxelEngine::Layer {
public:
  SimpleExample9()
      : m_Camera(70.0f, 0.000001f, 2500.0f), m_CameraPosition(0, 0, 0) {
    VE_PROFILE_FUNCTION;
    m_CameraPosition.z += 4;

    m_TerrainAtlas = VoxelEngine::TextureAtlas::Create();
    VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> dirt =
        VoxelEngine::TextureAtlas::CreateTextureSubImage(
            "assets/textures/texture_pack/assets/minecraft/textures/block/"
            "dirt.png");
    VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> grass_block_top =
        VoxelEngine::TextureAtlas::CreateTextureSubImage(
            "assets/textures/texture_pack/assets/minecraft/textures/block/"
            "grass_block_top.png");
    VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> grass_block_side =
        VoxelEngine::TextureAtlas::CreateTextureSubImage(
            "assets/textures/texture_pack/assets/minecraft/textures/block/"
            "grass_block_side.png");
    VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> grass_block_side_overlay =
        VoxelEngine::TextureAtlas::CreateTextureSubImage(
            "assets/textures/texture_pack/assets/minecraft/textures/block/"
            "grass_block_side_overlay.png");
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
    m_TerrainAtlas->Bind();
    // clang-format off
    m_DrawVertsCount=36;
float vertices[] = {

// BACK (z = 0)       , 0,
    0,1,0,  X(2), Y(2), 0,
    1,1,0,  X(3), Y(2), 0,
    1,0,0,  X(3), Y(1), 0,

    1,0,0,  X(3), Y(1), 1,
    0,0,0,  X(2), Y(1), 1,
    0,1,0,  X(2), Y(2), 1,
// RIGHT (x = 1)      , 0,
    1,1,1,  X(2), Y(2), 2,
    1,0,0,  X(3), Y(1), 2,
    1,1,0,  X(3), Y(2), 2,

    1,0,0,  X(3), Y(1), 3,
    1,1,1,  X(2), Y(2), 3,
    1,0,1,  X(2), Y(1), 3,
// BOTTOM (y = 0)     , 0,
    1,0,0,  X(2), Y(1), 4,
    1,0,1,  X(2), Y(2), 4,
    0,0,1,  X(1), Y(2), 4,

    0,0,1,  X(1), Y(2), 5,
    0,0,0,  X(1), Y(1), 5,
    1,0,0,  X(2), Y(1), 5,
// LEFT (x = 0)       , 0,
    0,1,1,  X(2), Y(2), 6,
    0,1,0,  X(3), Y(2), 6,
    0,0,0,  X(3), Y(1), 6,

    0,0,0,  X(3), Y(1), 7,
    0,0,1,  X(2), Y(1), 7,
    0,1,1,  X(2), Y(2), 7,
// FRONT (z = 1)
    0,1,1,  X(2), Y(2), 8,
    1,0,1,  X(3), Y(1), 8,
    1,1,1,  X(3), Y(2), 8,

    1,0,1,  X(3), Y(1), 9,
    0,1,1,  X(2), Y(2), 9,
    0,0,1,  X(2), Y(1), 9,
// TOP (y = 1)        , 0,
    0,1,1,  X(0), Y(3), 10,
    1,1,1,  X(1), Y(3), 10,
    1,1,0,  X(1), Y(2), 10,

    1,1,0,  X(1), Y(2), 11,
    0,1,0,  X(0), Y(2), 11,
    0,1,1,  X(0), Y(3), 11,


};
    // clang-format on
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    auto shader = m_ShaderLibrary.Load(
        "assets/shaders/SimpleExamples/SimpleExample1.glsl");
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  };
  ~SimpleExample9() { VE_PROFILE_FUNCTION; };
  void OnAttach() override {
    VE_PROFILE_FUNCTION;
    VoxelEngine::Application &application = VoxelEngine::Application::Get();
    application.GetWindow().SetMaximized(true);
    application.GetWindow().SetCursorVisibility(false);
    application.PushLayer<DebugOverlay>();
  };
  void OnUpdate(VoxelEngine::Timestep ts) override {
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

    VoxelEngine::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
    VoxelEngine::RenderCommand::Clear();
    VoxelEngine::Renderer::BeginScene(m_Camera);
    m_Camera.SetPosition(m_CameraPosition);

    auto shader = m_ShaderLibrary.Get("SimpleExample1");
    VoxelEngine::Renderer::Submit(
        shader, glm::translate(glm::mat4(1), glm::vec3(0, 0, -1)));
    shader->UploadUniformFloat(
        "time",
        VoxelEngine::Application::Get().GetWindow().GetTime() * timeMultiplier -
            timeOffset);
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, m_DrawVertsCount);

    VoxelEngine::Renderer::EndScene();
  };
  float X(int id) { return (16.0f * id) / m_TerrainAtlas->GetWidth(); }
  float Y(int id) { return (16.0f * id) / m_TerrainAtlas->GetHeight(); }
  void OnEvent(VoxelEngine::Event &event) override {
    VE_PROFILE_FUNCTION;
    VoxelEngine::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<VoxelEngine::KeyPressedEvent>(
        [&](VoxelEngine::KeyPressedEvent &e) {
          if (e.GetKeyCode() == VE_KEY_ESCAPE) {
            VoxelEngine::Application::Get().Close();
            return true;
          }
          if (e.GetKeyCode() == VE_KEY_Q && m_DrawVertsCount > 0) {
            m_DrawVertsCount -= 1;
            return true;
          }
          if (e.GetKeyCode() == VE_KEY_X) {
            timeOffset = VoxelEngine::Application::Get().GetWindow().GetTime();
            return true;
          }
          if (e.GetKeyCode() == VE_KEY_C) {
            timeMultiplier = 1.1;
            return true;
          }
          if (e.GetKeyCode() == VE_KEY_E && m_DrawVertsCount < 36) {
            m_DrawVertsCount += 1;
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
  };
  virtual void OnImGuiRender() override { VE_PROFILE_FUNCTION; };
  virtual void OnDetach() override { VE_PROFILE_FUNCTION; };

  inline VoxelEngine::PerspectiveCamera &GetCamera() { return m_Camera; }

private:
  VoxelEngine::ShaderLibrary m_ShaderLibrary;
  VoxelEngine::PerspectiveCamera m_Camera;
  float timeOffset = 0;
  float timeMultiplier = 1;
  // glm::vec3 m_SquarePosition;
  glm::vec3 m_CameraPosition;
  float m_CameraMoveSpeed = 1.0f;
  unsigned int m_VAO;
  int m_DrawVertsCount;
  VoxelEngine::Ref<VoxelEngine::Texture2D> m_Texture;
  VoxelEngine::Ref<VoxelEngine::TextureAtlas> m_TerrainAtlas;
};
