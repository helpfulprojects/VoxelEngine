#pragma once
#include <VoxelEngine.h>
#include <glad/glad.h>
class SimpleExample8 : public VoxelEngine::Layer {
public:
	SimpleExample8()
		: m_Camera(70.0f, 0.1f, 1500.0f),
		m_CameraPosition(0, 0, 0)
	{
		VE_PROFILE_FUNCTION;
		float vertices[] = {
			// first triangle
			 0.5f,  0.5f, 0.0f,  // top right
			 0.5f, -0.5f, 0.0f,  // bottom right
			-0.5f,  0.5f, 0.0f,  // top left 
			// second triangle
			 0.5f, -0.5f, 0.0f,  // bottom right
			-0.5f, -0.5f, 0.0f,  // bottom left
			-0.5f,  0.5f, 0.0f   // top left
		};
		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		unsigned int VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		auto shader = m_ShaderLibrary.Load("assets/shaders/SimpleExamples/SimpleExample8.glsl");
	};
	~SimpleExample8() {
		VE_PROFILE_FUNCTION;
	};
	void OnAttach() override {
		VE_PROFILE_FUNCTION;
		VoxelEngine::Application& application = VoxelEngine::Application::Get();
		application.GetWindow().SetMaximized(true);
		application.GetWindow().SetCursorVisibility(false);
	};
	void OnUpdate(VoxelEngine::Timestep ts) override {
		VE_PROFILE_FUNCTION;

		{
			VE_PROFILE_SCOPE("Key pooling");
			if (VoxelEngine::Input::IsKeyPressed(VE_KEY_A)) {
				m_CameraPosition -= glm::normalize(m_Camera.GetRight() * glm::vec3(1, 0, 1)) * (m_CameraMoveSpeed * ts);
			}
			else if (VoxelEngine::Input::IsKeyPressed(VE_KEY_D)) {
				m_CameraPosition += glm::normalize(m_Camera.GetRight() * glm::vec3(1, 0, 1)) * (m_CameraMoveSpeed * ts);
			}
			if (VoxelEngine::Input::IsKeyPressed(VE_KEY_W)) {
				m_CameraPosition += glm::normalize(m_Camera.GetFront() * glm::vec3(1, 0, 1)) * (m_CameraMoveSpeed * ts);
			}
			else if (VoxelEngine::Input::IsKeyPressed(VE_KEY_S)) {
				m_CameraPosition -= glm::normalize(m_Camera.GetFront() * glm::vec3(1, 0, 1)) * (m_CameraMoveSpeed * ts);
			}

			if (VoxelEngine::Input::IsKeyPressed(VE_KEY_SPACE)) {
				m_CameraPosition += glm::vec3(0.0f, 1.0f, 0.0f) * (m_CameraMoveSpeed * ts);
			}
			else if (VoxelEngine::Input::IsKeyPressed(VE_KEY_LEFT_SHIFT)) {
				m_CameraPosition -= glm::vec3(0.0f, 1.0f, 0.0f) * (m_CameraMoveSpeed * ts);
			}
		}

		VoxelEngine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		VoxelEngine::RenderCommand::Clear();
		VoxelEngine::Renderer::BeginScene(m_Camera);
		m_Camera.SetPosition(m_CameraPosition);

		auto shader = m_ShaderLibrary.Get("SimpleExample8");
		VoxelEngine::Renderer::Submit(shader, glm::translate(glm::mat4(1), glm::vec3(0, 0, -1)));
		glBindVertexArray(m_VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		VoxelEngine::Renderer::EndScene();
	};
	void OnEvent(VoxelEngine::Event& event) override {
		VE_PROFILE_FUNCTION;
		VoxelEngine::EventDispatcher dispatcher(event);
		dispatcher.Dispatch<VoxelEngine::KeyPressedEvent>([](VoxelEngine::KeyPressedEvent& e) {
			if (e.GetKeyCode() == VE_KEY_ESCAPE) {
				VoxelEngine::Application::Get().Close();
				return true;
			}
			return false;
			});
		dispatcher.Dispatch<VoxelEngine::MouseMovedEvent>([&](VoxelEngine::MouseMovedEvent& e) {
			m_Camera.AddToYawAndPitch(e.GetXOffset(), e.GetYOffset());
			return true;
			});
		dispatcher.Dispatch<VoxelEngine::WindowResizeEvent>([&](VoxelEngine::WindowResizeEvent& e) {
			m_Camera.RecalculateProjectionMatrix();
			return false;
			});
	};
	virtual void OnImGuiRender() override {
		VE_PROFILE_FUNCTION;
	};
	virtual void OnDetach() override {
		VE_PROFILE_FUNCTION;
	};

	inline VoxelEngine::PerspectiveCamera& GetCamera() { return m_Camera; }
private:
	VoxelEngine::ShaderLibrary m_ShaderLibrary;
	VoxelEngine::PerspectiveCamera m_Camera;
	//glm::vec3 m_SquarePosition;
	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 5.0f;
	unsigned int m_VAO;
};
