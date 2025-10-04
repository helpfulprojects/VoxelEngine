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
		m_Camera(-1.6f, 1.6f, -0.9f, 0.9f),
		m_SquarePosition(0.0f),
		m_CameraPosition(0.0f)
	{

		m_VertexArray.reset(VoxelEngine::VertexArray::Create());

		float vertices[] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.2f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f,
		};

		VoxelEngine::Ref<VoxelEngine::VertexBuffer> triangleVB(VoxelEngine::VertexBuffer::Create(vertices, sizeof(vertices)));
		VoxelEngine::BufferLayout layout = {
			{VoxelEngine::ShaderDataType::Float3, "a_Position"},
			{VoxelEngine::ShaderDataType::Float4, "a_Color"},
		};
		triangleVB->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(triangleVB);
		uint32_t indices[3] = { 0,1,2 };
		VoxelEngine::Ref<VoxelEngine::IndexBuffer> triangleIB(VoxelEngine::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(triangleIB);

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


		std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec4 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec4 v_Color;

			void main()
			{
				v_Color = a_Color;
				gl_Position = u_ViewProjection*u_Transform*a_Position;
			}
		)";
		std::string fragmentSrc = R"(
			#version 330 core

			out vec4 color;
			in vec4 v_Color;

			void main()
			{
				color = v_Color;
			}
		)";

		m_Shader = (VoxelEngine::Shader::Create("triangleShader", vertexSrc, fragmentSrc));

		std::string flatColorShaderVertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec4 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			void main()
			{
				gl_Position = u_ViewProjection*u_Transform*a_Position;
			}
		)";
		std::string flatColorShaderFragmentSrc = R"(
			#version 330 core

			out vec4 color;
			uniform vec3 u_Color;

			void main()
			{
				color = vec4(u_Color,1.0);
			}
		)";

		m_FlatColorShader = VoxelEngine::Shader::Create("flatColor", flatColorShaderVertexSrc, flatColorShaderFragmentSrc);

		std::string textureShaderVertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec4 a_Position;
			layout(location = 1) in vec2 a_TexCoord;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec2 v_TexCoord;

			void main()
			{
				v_TexCoord = a_TexCoord;
				gl_Position = u_ViewProjection*u_Transform*a_Position;
			}
		)";
		std::string textureShaderFragmentSrc = R"(
			#version 330 core

			out vec4 color;
			in vec2 v_TexCoord;

			uniform sampler2D u_Texture;

			void main()
			{
				color = texture(u_Texture,v_TexCoord);
			}
		)";

		auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

		m_Texture = VoxelEngine::Texture2D::Create("assets/textures/Checkerboard.png");
		m_ChernoLogoTexture = VoxelEngine::Texture2D::Create("assets/textures/ChernoLogo.png");

		textureShader->Bind();
		std::dynamic_pointer_cast<VoxelEngine::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);
	}
	void OnUpdate(VoxelEngine::Timestep ts) override {
		TracyNoop;
		VE_PROFILE_FUNCTION();

		{
			VE_PROFILE_SCOPE("Key pooling");
			if (VoxelEngine::Input::IsKeyPressed(VE_KEY_A)) {
				m_CameraPosition.x -= m_CameraMoveSpeed * ts;
			}
			else if (VoxelEngine::Input::IsKeyPressed(VE_KEY_D)) {
				m_CameraPosition.x += m_CameraMoveSpeed * ts;
			}
			if (VoxelEngine::Input::IsKeyPressed(VE_KEY_W)) {
				m_CameraPosition.y += m_CameraMoveSpeed * ts;
			}
			else if (VoxelEngine::Input::IsKeyPressed(VE_KEY_S)) {
				m_CameraPosition.y -= m_CameraMoveSpeed * ts;
			}

			if (VoxelEngine::Input::IsKeyPressed(VE_KEY_Q)) {
				m_CameraRotation += m_CameraRotationSpeed * ts;
			}
			else if (VoxelEngine::Input::IsKeyPressed(VE_KEY_E)) {
				m_CameraRotation -= m_CameraRotationSpeed * ts;
			}
		}

		{
			VE_PROFILE_SCOPE("Draw");
			VoxelEngine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
			VoxelEngine::RenderCommand::Clear();
			VoxelEngine::Renderer::BeginScene(m_Camera);

			m_Camera.SetPosition(m_CameraPosition);
			m_Camera.SetRotation(m_CameraRotation);

			glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(0.1f));

			m_FlatColorShader->Bind();
			std::dynamic_pointer_cast<VoxelEngine::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);
			for (int y = 0; y < 20; y++) {
				for (int x = 0; x < 20; x++) {
					glm::vec3 pos(x * 0.11f, y * 0.11f, 0);
					glm::mat4 transform = glm::translate(glm::mat4(1), pos) * scale;
					VoxelEngine::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
				}
			}
			auto textureShader = m_ShaderLibrary.Get("Texture");
			m_Texture->Bind();
			VoxelEngine::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1), glm::vec3(1.5f)));
			m_ChernoLogoTexture->Bind();
			VoxelEngine::Renderer::Submit(textureShader, m_SquareVA,
				glm::scale(glm::mat4(1), glm::vec3(1.5f)));
			//Triangle
			//VoxelEngine::Renderer::Submit(m_Shader, m_VertexArray);

			VoxelEngine::Renderer::EndScene();
		}
	}
	void OnEvent(VoxelEngine::Event& event) override {
	}

	virtual void OnImGuiRender() override {
		//ImGui::Begin("Settings");
		//ImGui::End();
	}

private:
	VoxelEngine::ShaderLibrary m_ShaderLibrary;
	VoxelEngine::Ref<VoxelEngine::Shader> m_Shader;
	VoxelEngine::Ref<VoxelEngine::Shader> m_FlatColorShader;
	VoxelEngine::Ref<VoxelEngine::VertexArray> m_VertexArray;
	VoxelEngine::Ref<VoxelEngine::VertexArray> m_SquareVA;
	VoxelEngine::Ref<VoxelEngine::Texture2D> m_Texture, m_ChernoLogoTexture;
	VoxelEngine::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraRotation = 0.0f;
	float m_CameraMoveSpeed = 5.0f;
	float m_CameraRotationSpeed = 90.0f;
	glm::vec3 m_SquarePosition;
	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.7f };
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
