#include <VoxelEngine.h>
#include "imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <Platform/OpenGL/OpenGLShader.h>
#include <glm/gtc/type_ptr.hpp>
#include <chrono>
#include <tracy/Tracy.hpp>
class ExampleLayer : public VoxelEngine::Layer {
public:
	ExampleLayer()
		:Layer("Example"),
		m_Camera(70.0f, 0.1f, 1500.0f),
		m_SquarePosition(0, 0, -1),
		m_CameraPosition(0, 0, 0)
	{
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

		auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

		m_Texture = VoxelEngine::Texture2D::Create("assets/textures/Checkerboard.png");
		m_ChernoLogoTexture = VoxelEngine::Texture2D::Create("assets/textures/ChernoLogo.png");

		textureShader->Bind();
		std::dynamic_pointer_cast<VoxelEngine::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);

	}
	void OnAttach() override {
		VE_PROFILE_FUNCTION;
		VoxelEngine::Application::Get().GetWindow().SetMaximized(true);
		VoxelEngine::Application::Get().GetWindow().SetCursorVisibility(false);
	}
	void OnUpdate(VoxelEngine::Timestep ts) override {
		VE_PROFILE_FUNCTION;

		{
			VE_PROFILE_SCOPE("Key pooling");
			if (VoxelEngine::Input::IsKeyPressed(VE_KEY_A)) {
				m_CameraPosition -= m_Camera.GetWorldRight() * (m_CameraMoveSpeed * ts);
			}
			else if (VoxelEngine::Input::IsKeyPressed(VE_KEY_D)) {
				m_CameraPosition += m_Camera.GetWorldRight() * (m_CameraMoveSpeed * ts);
			}
			if (VoxelEngine::Input::IsKeyPressed(VE_KEY_W)) {
				m_CameraPosition += m_Camera.GetWorldFront() * (m_CameraMoveSpeed * ts);
			}
			else if (VoxelEngine::Input::IsKeyPressed(VE_KEY_S)) {
				m_CameraPosition -= m_Camera.GetWorldFront() * (m_CameraMoveSpeed * ts);
			}

			if (VoxelEngine::Input::IsKeyPressed(VE_KEY_SPACE)) {
				m_CameraPosition += m_Camera.GetWorldUp() * (m_CameraMoveSpeed * ts);
			}
			else if (VoxelEngine::Input::IsKeyPressed(VE_KEY_LEFT_SHIFT)) {
				m_CameraPosition -= m_Camera.GetWorldUp() * (m_CameraMoveSpeed * ts);
			}
		}

		{
			VE_PROFILE_SCOPE("Draw");
			VoxelEngine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
			VoxelEngine::RenderCommand::Clear();
			VoxelEngine::Renderer::BeginScene(m_Camera);

			m_Camera.SetPosition(m_CameraPosition);

			auto textureShader = m_ShaderLibrary.Get("Texture");
			m_Texture->Bind();
			VoxelEngine::Renderer::Submit(textureShader, m_SquareVA,
				glm::translate(glm::mat4(1), m_SquarePosition) * glm::scale(glm::mat4(1), glm::vec3(1.5f))
			);
			m_ChernoLogoTexture->Bind();
			VoxelEngine::Renderer::Submit(textureShader, m_SquareVA,
				glm::translate(glm::mat4(1), m_SquarePosition) * glm::scale(glm::mat4(1), glm::vec3(1.5f))
			);

			VoxelEngine::Renderer::EndScene();
		}
	}
	void OnEvent(VoxelEngine::Event& event) override {
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
	}

	virtual void OnImGuiRender() override {
		//ImGui::Begin("Settings");
		//ImGui::End();
	}

private:
	VoxelEngine::ShaderLibrary m_ShaderLibrary;
	VoxelEngine::Ref<VoxelEngine::VertexArray> m_SquareVA;
	VoxelEngine::Ref<VoxelEngine::Texture2D> m_Texture, m_ChernoLogoTexture;
	VoxelEngine::PerspectiveCamera m_Camera;
	glm::vec3 m_SquarePosition;
	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 5.0f;
};
class MinecraftClone : public VoxelEngine::Application {
public:
	MinecraftClone() {
		PushLayer(new ExampleLayer());
	}
	~MinecraftClone() {

	}
};
VoxelEngine::Application* VoxelEngine::CreateApplication() {
	return new MinecraftClone();
}
