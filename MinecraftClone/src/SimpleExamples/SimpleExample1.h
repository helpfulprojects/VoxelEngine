#pragma once
#include <VoxelEngine.h>
#include <glad/glad.h>
class SimpleExample1 : public VoxelEngine::Layer {
public:
  SimpleExample1() : m_Camera(70.0f, 0.1f, 1500.0f), m_CameraPosition(0, 0, 0) {
    VE_PROFILE_FUNCTION;
    // clang-format off
    m_DrawVertsCount=0;
    float vertices[] = {
	//FRONT
	-1,1,1,
	 1,1,1,
	-1,-1,1,
	 1,-1,1,
	1.f, -1.f, -1.f,    // Back-bottom-right
	1.f, 1.f, 1.f,      // Front-top-right
	1.f, 1.f, -1.f,     // Back-top-right
	-1.f, 1.f, 1.f,     // Front-top-left
	 -1.f, 1.f, -1.f,    // Back-top-left
	 -1.f, -1.f, 1.f,    // Front-bottom-left
	 -1.f, -1.f, -1.f,   // Back-bottom-left
	 1.f, -1.f, -1.f,    // Back-bottom-right
	 -1.f, 1.f, -1.f,    // Back-top-left
	 1.f, 1.f, -1.f      // Back-top-right
    };
    // clang-format on
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    auto shader = m_ShaderLibrary.Load(
        "assets/shaders/SimpleExamples/SimpleExample1.glsl");
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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

    auto shader = m_ShaderLibrary.Get("SimpleExample1");
    VoxelEngine::Renderer::Submit(
        shader, glm::translate(glm::mat4(1), glm::vec3(0, 0, -1)));
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, m_DrawVertsCount);

    VoxelEngine::Renderer::EndScene();
  };
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
  float m_CameraMoveSpeed = 5.0f;
  unsigned int m_VAO;
  int m_DrawVertsCount;
};
