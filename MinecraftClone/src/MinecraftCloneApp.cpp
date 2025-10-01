#include <VoxelEngine.h>
#include "imgui.h"
class ExampleLayer : public VoxelEngine::Layer {
public:
	ExampleLayer()
		:Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f),
		m_CameraPosition(0.0f)
	{

		m_VertexArray.reset(VoxelEngine::VertexArray::Create());

		float vertices[] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.2f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f,
		};

		std::shared_ptr<VoxelEngine::VertexBuffer> triangleVB(VoxelEngine::VertexBuffer::Create(vertices, sizeof(vertices)));
		VoxelEngine::BufferLayout layout = {
			{VoxelEngine::ShaderDataType::Float3, "a_Position"},
			{VoxelEngine::ShaderDataType::Float4, "a_Color"},
		};
		triangleVB->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(triangleVB);
		uint32_t indices[3] = { 0,1,2 };
		std::shared_ptr<VoxelEngine::IndexBuffer> triangleIB(VoxelEngine::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(triangleIB);

		m_SquareVA.reset(VoxelEngine::VertexArray::Create());
		float squareVertices[] = {
			-0.75f, -0.75f, 0.0f,
			 0.75f, -0.75f, 0.0f,
			 0.75f,  0.75f, 0.0f,
			 -0.75f,  0.75f, 0.0f,
		};
		std::shared_ptr<VoxelEngine::VertexBuffer> squareVB(VoxelEngine::VertexBuffer::Create(squareVertices, sizeof(vertices)));
		VoxelEngine::BufferLayout squareLayout = {
			{VoxelEngine::ShaderDataType::Float3, "a_Position"},
		};
		squareVB->SetLayout(squareLayout);
		m_SquareVA->AddVertexBuffer(squareVB);
		uint32_t squareIndices[] = {
			0,1,2,
			2,3,0,
		};
		std::shared_ptr<VoxelEngine::IndexBuffer> squareIB(VoxelEngine::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);


		std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec4 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;

			out vec4 v_Color;

			void main()
			{
				v_Color = a_Color;
				gl_Position = u_ViewProjection*a_Position;
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

		m_Shader.reset(new VoxelEngine::Shader(vertexSrc, fragmentSrc));

		std::string blueShaderVertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec4 a_Position;

			uniform mat4 u_ViewProjection;

			void main()
			{
				gl_Position = u_ViewProjection*a_Position;
			}
		)";
		std::string blueShaderFragmentSrc = R"(
			#version 330 core

			out vec4 color;

			void main()
			{
				color = vec4(0.2,0.3,0.8,1.0);
			}
		)";

		m_BlueShader.reset(new VoxelEngine::Shader(blueShaderVertexSrc, blueShaderFragmentSrc));
	}
	void OnUpdate() override {
		if (VoxelEngine::Input::IsKeyPressed(VE_KEY_A)) {
			m_CameraPosition.x -= m_CameraMoveSpeed;
		}
		else if (VoxelEngine::Input::IsKeyPressed(VE_KEY_D)) {
			m_CameraPosition.x += m_CameraMoveSpeed;
		}
		if (VoxelEngine::Input::IsKeyPressed(VE_KEY_W)) {
			m_CameraPosition.y += m_CameraMoveSpeed;
		}
		else if (VoxelEngine::Input::IsKeyPressed(VE_KEY_S)) {
			m_CameraPosition.y -= m_CameraMoveSpeed;
		}

		if (VoxelEngine::Input::IsKeyPressed(VE_KEY_Q)) {
			m_CameraRotation += m_CameraRotationSpeed;
		}
		else if (VoxelEngine::Input::IsKeyPressed(VE_KEY_E)) {
			m_CameraRotation -= m_CameraRotationSpeed;
		}

		VoxelEngine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		VoxelEngine::RenderCommand::Clear();
		VoxelEngine::Renderer::BeginScene(m_Camera);

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);
		VoxelEngine::Renderer::Submit(m_BlueShader, m_SquareVA);

		VoxelEngine::Renderer::Submit(m_Shader, m_VertexArray);

		VoxelEngine::Renderer::EndScene();
	}
	void OnEvent(VoxelEngine::Event& event) override {
	}

private:
	std::shared_ptr<VoxelEngine::Shader> m_Shader;
	std::shared_ptr<VoxelEngine::Shader> m_BlueShader;
	std::shared_ptr<VoxelEngine::VertexArray> m_VertexArray;
	std::shared_ptr<VoxelEngine::VertexArray> m_SquareVA;
	VoxelEngine::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 0.1f;
	float m_CameraRotationSpeed = 0.2f;
	float m_CameraRotation = 0.0f;
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
