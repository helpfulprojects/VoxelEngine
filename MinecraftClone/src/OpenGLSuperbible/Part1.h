#pragma once
#include <VoxelEngine.h>
#include <glad/glad.h>
class Part1 : public VoxelEngine::Layer {
public:
  Part1() {};
  ~Part1() {};
  void OnAttach() override {
    VoxelEngine::Application &application = VoxelEngine::Application::Get();
    application.GetWindow().SetMaximized(true);
    m_ShaderLibrary.Load("assets/shaders/OpenGLSuperbible/Part1.glsl");
    uint32_t vao;
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glPointSize(40.0f);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //  glEnableVertexArrayAttrib(vao, 0);
  };
  void OnUpdate(VoxelEngine::Timestep ts) override {
    VoxelEngine::Application &application = VoxelEngine::Application::Get();
    float currentTime = application.GetWindow().GetTime();
    GLfloat red[] = {0.0f, 0.2f, 0.0f, 1.0f};
    GLfloat attrib[] = {(float)sin(currentTime) * 0.5f,
                        (float)cos(currentTime) * 0.6f, 0.0f, 0.0f};
    glVertexAttrib4fv(0, attrib);
    GLfloat color[] = {0.0f, 0.8f, 1.0f, 1.0f};
    glVertexAttrib4fv(1, color);

    auto shader = m_ShaderLibrary.Get("Part1");
    glClearBufferfv(GL_COLOR, 0, red);
    shader->Bind();
    // glDrawArrays(GL_TRIANGLES, 0, 3);
    glDrawArrays(GL_PATCHES, 0, 3);
  };
  void OnEvent(VoxelEngine::Event &event) override {
    VoxelEngine::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<VoxelEngine::KeyPressedEvent>(
        [](VoxelEngine::KeyPressedEvent &e) {
          if (e.GetKeyCode() == VE_KEY_ESCAPE) {
            VoxelEngine::Application::Get().Close();
            return true;
          }
          return false;
        });
  };
  virtual void OnImGuiRender() override {};
  virtual void OnDetach() override {};

private:
  VoxelEngine::ShaderLibrary m_ShaderLibrary;
};
