#pragma once
#include <VoxelEngine.h>
#include <glad/glad.h>
class SimpleExample7 : public VoxelEngine::Layer {
public:
	SimpleExample7()
		: m_Camera(70.0f, 0.1f, 1500.0f),
		m_CameraPosition(0, 0, 0)
	{
		VE_PROFILE_FUNCTION;
		auto shader = m_ShaderLibrary.Load("assets/shaders/SimpleExamples/SimpleExample7.glsl");

		m_SquareVA.reset(VoxelEngine::VertexArray::Create());
		float squareVertices[] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1000.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1000.0f, 1000.0f,
			 -0.5f,  0.5f, 0.0f, 0.0f, 1000.0f
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

		VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> dirt = VoxelEngine::TextureAtlas::CreateTextureSubImage("assets/textures/texture_pack/assets/minecraft/textures/block/dirt.png");
		uint32_t m_RendererID;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		int mipLevels = 1 + (int)std::floor(std::log2(std::max(16, 16)));
		glTextureStorage2D(m_RendererID, mipLevels, GL_RGBA8, dirt->GetWidth(), dirt->GetHeight());
		glTextureSubImage2D(m_RendererID, 0, 0, 0, dirt->GetWidth(), dirt->GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, dirt->GetData());
		glGenerateTextureMipmap(m_RendererID);
		//glTextureParameteri(m_RendererID, GL_TEXTURE_MAX_LEVEL, 4);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glBindTextureUnit(0, m_RendererID);



		//glGenTextures(1, &m_RendererID);
		//glBindTexture(GL_TEXTURE_2D, m_RendererID);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		////glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, dirt->GetWidth(), dirt->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, dirt->GetData());
		//glGenerateMipmap(GL_TEXTURE_2D);

	};
	~SimpleExample7() {
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

		auto shader = m_ShaderLibrary.Get("SimpleExample7");
		VoxelEngine::Renderer::Submit(shader, m_SquareVA,
			glm::translate(glm::mat4(1), glm::vec3(0, 0, -1)) * glm::scale(glm::mat4(1), glm::vec3(1000))
		);

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
	VoxelEngine::PerspectiveCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 5.0f;
};
