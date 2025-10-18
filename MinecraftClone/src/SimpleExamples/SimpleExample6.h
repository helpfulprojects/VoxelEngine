#pragma once
#include <VoxelEngine.h>
#include <glad/glad.h>
class SimpleExample6 : public VoxelEngine::Layer {
public:
	SimpleExample6()
		: m_Camera(70.0f, 0.1f, 1500.0f),
		m_CameraPosition(0, 0, 0)
	{
		VE_PROFILE_FUNCTION;

		auto computeShader = m_ShaderLibrary.Load("assets/shaders/SimpleExamples/SimpleExample6Compute.glsl");
		m_Texture = VoxelEngine::Texture2D::Create(m_TEXTURE_WIDTH, m_TEXTURE_HEIGHT);
		m_Texture->BindImageTexture();
		auto shader = m_ShaderLibrary.Load("assets/shaders/SimpleExamples/SimpleExample6.glsl");

		m_SquareVA.reset(VoxelEngine::VertexArray::Create());
		float squareVertices[] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			 -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};
		VoxelEngine::Ref<VoxelEngine::VertexBuffer> squareVB(VoxelEngine::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		VoxelEngine::BufferLayout squareLayout = {
			{VoxelEngine::ShaderDataType::Float3, "a_Position"},
			{VoxelEngine::ShaderDataType::Float2, "a_TexCoord"},
		};
		squareVB->SetLayout(squareLayout);
		m_SquareVA->AddVertexBuffer(squareVB);
		uint32_t squareIndices[] = {
			0,1,2,
			2,3,0,
		};
		VoxelEngine::Ref<VoxelEngine::IndexBuffer> squareIB(VoxelEngine::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);
	};
	~SimpleExample6() {
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
		auto computeShader = m_ShaderLibrary.Get("SimpleExample6Compute");
		computeShader->Bind();
		VoxelEngine::Window& window = VoxelEngine::Application::Get().GetWindow();
		computeShader->UploadUniformFloat("t", window.GetTime());
		glDispatchCompute((unsigned int)m_TEXTURE_WIDTH / 10, (unsigned int)m_TEXTURE_HEIGHT / 10, 1);

		// make sure writing to image has finished before read
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		auto shader = m_ShaderLibrary.Get("SimpleExample6");
		VoxelEngine::Renderer::Submit(shader, m_SquareVA, glm::translate(glm::mat4(1), glm::vec3(0, 0, -1)));
		//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

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
	VoxelEngine::Ref<VoxelEngine::VertexArray> m_SquareVA;
	VoxelEngine::Ref<VoxelEngine::Texture2D> m_Texture;
	VoxelEngine::PerspectiveCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 5.0f;
	unsigned int m_VAO;
	const unsigned int m_TEXTURE_WIDTH = 1000, m_TEXTURE_HEIGHT = 1000;
};
