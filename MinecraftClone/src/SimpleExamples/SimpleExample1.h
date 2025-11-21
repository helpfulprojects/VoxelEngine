#pragma once
#include <filesystem>
#include <VoxelEngine.h>
#include <glad/glad.h>
class SimpleExample1 : public VoxelEngine::Layer {
public:
  SimpleExample1() : m_Camera(70.0f, 0.1f, 1500.0f), m_CameraPosition(0, 0, 0) {
    VE_PROFILE_FUNCTION;
    std::string blocksFolderLocation =
        "assets/textures/texture_pack/assets/minecraft/textures/block/";
    std::string blocksFolderLocationAlternative =
        "assets/textures/texture_pack/assets/minecraft/textures/blocks/";
    std::string selectedFolder;
    if (std::filesystem::exists(blocksFolderLocation)) {
      selectedFolder = blocksFolderLocation;
    } else if (std::filesystem::exists(blocksFolderLocationAlternative)) {
      selectedFolder = blocksFolderLocationAlternative;
    } else {
      VE_ERROR("No valid block texture folder found!");
    }
    std::string grassBlockPrefix = "grass_block";
    if (!std::filesystem::exists(selectedFolder + grassBlockPrefix +
                                 "_top.png")) {
      grassBlockPrefix = "grass";
    }
    {
      VE_PROFILE_SCOPE("Bake tnt atlas");
      m_TntAtlas = VoxelEngine::TextureAtlas::Create();
      VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> tnt_side00 =
          VoxelEngine::TextureAtlas::CreateTextureSubImage(selectedFolder +
                                                           "tnt_side.png");
      tnt_side00->SetOffset(0, 0);
      tnt_side00->Rotate(180);
      VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> tnt_bottom01 =
          VoxelEngine::TextureAtlas::CreateTextureSubImage(selectedFolder +
                                                           "tnt_bottom.png");
      tnt_bottom01->SetOffset(0, 1);
      VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> tnt_side11 =
          VoxelEngine::TextureAtlas::CreateTextureSubImage(selectedFolder +
                                                           "tnt_side.png");
      tnt_side11->SetOffset(1, 1);
      tnt_side11->Rotate(270);
      VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> tnt_top21 =
          VoxelEngine::TextureAtlas::CreateTextureSubImage(selectedFolder +
                                                           "tnt_top.png");
      tnt_top21->SetOffset(2, 1);
      VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> tnt_side31 =
          VoxelEngine::TextureAtlas::CreateTextureSubImage(selectedFolder +
                                                           "tnt_side.png");
      tnt_side31->SetOffset(3, 1);
      tnt_side31->Rotate(90);
      VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> tnt_bottom41 =
          VoxelEngine::TextureAtlas::CreateTextureSubImage(selectedFolder +
                                                           "tnt_bottom.png");
      tnt_bottom41->SetOffset(4, 1);
      VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> tnt_side42 =
          VoxelEngine::TextureAtlas::CreateTextureSubImage(selectedFolder +
                                                           "tnt_side.png");
      tnt_side42->SetOffset(4, 2);
      m_TntAtlas->Add(tnt_side00);
      m_TntAtlas->Add(tnt_bottom01);
      m_TntAtlas->Add(tnt_side11);
      m_TntAtlas->Add(tnt_top21);
      m_TntAtlas->Add(tnt_side31);
      m_TntAtlas->Add(tnt_bottom41);
      m_TntAtlas->Add(tnt_side42);
      m_TntAtlas->Bake(8);
      m_TntAtlas->Bind();
    }
    // clang-format off
    m_DrawVertsCount=14;
    float vertices[] = {
	 1,1,1,   X(5),Y(3),
	0,1,1,   X(4),Y(3),
	 1,0,1,  X(5),Y(2),
	0,0,1,  X(4),Y(2),
	0,0,0, X(4),Y(1),
	0,1,1,   X(3),Y(2), 
	0,1,0,  X(3),Y(1), 
	1,1,1,    X(2),Y(2), 
	1,1,0,   X(2),Y(1),
	1,0,1,   X(1),Y(2),
	1,0,0,  X(1),Y(1),
	0,0,0, X(0),Y(1),
	1,1,0,   X(1),Y(0),
	0,1,0,  X(0),Y(0),
    };
    // clang-format on
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    auto shader = m_ShaderLibrary.Load(
        "assets/shaders/SimpleExamples/SimpleExample2.glsl");
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  };
  ~SimpleExample1() { VE_PROFILE_FUNCTION; };
  void OnAttach() override {
    VE_PROFILE_FUNCTION;
    VoxelEngine::Application &application = VoxelEngine::Application::Get();
    application.GetWindow().SetMaximized(true);
    application.GetWindow().SetCursorVisibility(false);
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

    auto shader = m_ShaderLibrary.Get("SimpleExample2");
    VoxelEngine::Renderer::Submit(
        shader, glm::translate(glm::mat4(1), glm::vec3(0, 0, -1)));
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, m_DrawVertsCount);

    VoxelEngine::Renderer::EndScene();
  };
  float X(int id) { return (16.0f * id) / 128.0f; }
  float Y(int id) { return (16.0f * id) / 128.0f; }
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
          if (e.GetKeyCode() == VE_KEY_E && m_DrawVertsCount < 14) {
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
  // glm::vec3 m_SquarePosition;
  glm::vec3 m_CameraPosition;
  float m_CameraMoveSpeed = 0.5f;
  unsigned int m_VAO;
  int m_DrawVertsCount;
  VoxelEngine::Ref<VoxelEngine::Texture2D> m_Texture;
  VoxelEngine::Ref<VoxelEngine::TextureAtlas> m_TntAtlas;
};
