#pragma once
#include <VoxelEngine.h>
#include <glad/glad.h>
class SimpleExample3 : public VoxelEngine::Layer {
public:
	SimpleExample3()
		: m_Camera(70.0f, 0.1f, 1500.0f),
		m_CameraPosition(0, 0, 0)
	{
		VE_PROFILE_FUNCTION;

		uint32_t ssbo;
		glCreateBuffers(1, &ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		std::vector<glm::vec2> ssboVertices;
		ssboVertices.push_back({ 0.0f,0.0f });
		ssboVertices.push_back({ 1.1f,0.0f });
		ssboVertices.push_back({ 2.2f,0.0f });
		ssboVertices.push_back({ 3.3f,0.0f });
		ssboVertices.push_back({ 4.4f,0.0f });
		ssboVertices.push_back({ 5.5f,0.0f });
		glBufferData(GL_SHADER_STORAGE_BUFFER, ssboVertices.size() * sizeof(glm::vec2), ssboVertices.data(), GL_STATIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

		auto shader = m_ShaderLibrary.Load("assets/shaders/SimpleExamples/SimpleExample3.glsl");
	};
	~SimpleExample3() {
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

		auto shader = m_ShaderLibrary.Get("SimpleExample3");
		VoxelEngine::Renderer::Submit(shader, glm::translate(glm::mat4(1), glm::vec3(0, 0, -1)));
		glDrawArrays(GL_TRIANGLES, 0, 5 * 6);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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
