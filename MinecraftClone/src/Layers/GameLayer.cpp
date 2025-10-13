#include "GameLayer.h"
#include "../Overlays/DebugOverlay.h"
#include <Platform/OpenGL/OpenGLShader.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

GameLayer::GameLayer()
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

	//SSBO
	auto ssboShader = m_ShaderLibrary.Load("assets/shaders/Ssbo.glsl");

	uint32_t ssbo;
	m_SsboVao.reset(VoxelEngine::VertexArray::Create());
	m_SsboVao->Bind();
	glCreateBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	std::vector<int> ssboVertices;

	glm::ivec3 position = glm::ivec3(0);
	int vertex = (position.x | position.y << 10 | position.z << 20);
	ssboVertices.push_back(vertex);

	//glNamedBufferStorage(ssbo, ssboVertices.size() * sizeof(int), ssboVertices.data(), NULL);
	glBufferData(GL_SHADER_STORAGE_BUFFER, ssboVertices.size() * sizeof(int), ssboVertices.data(), NULL);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

}
GameLayer::~GameLayer()
{
}
void GameLayer::OnAttach() {
	VE_PROFILE_FUNCTION;
	VoxelEngine::Application& application = VoxelEngine::Application::Get();
	application.GetWindow().SetMaximized(true);
	application.GetWindow().SetCursorVisibility(false);
	application.PushLayer<DebugOverlay>();
}
void GameLayer::OnUpdate(VoxelEngine::Timestep ts) {
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

	{
		VE_PROFILE_SCOPE("Draw");
		VoxelEngine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		VoxelEngine::RenderCommand::Clear();
		VoxelEngine::Renderer::BeginScene(m_Camera);

		m_Camera.SetPosition(m_CameraPosition);

		//auto textureShader = m_ShaderLibrary.Get("Texture");
		//m_Texture->Bind();
		auto ssboShader = m_ShaderLibrary.Get("Ssbo");
		VoxelEngine::Renderer::Submit(ssboShader, m_SsboVao,
			glm::translate(glm::mat4(1), glm::vec3(0, 0, -1))
		);
		glDrawArrays(GL_TRIANGLES, 0, 1 * 6);
		//m_ChernoLogoTexture->Bind();
		//VoxelEngine::Renderer::Submit(textureShader, m_SquareVA,
		//	glm::translate(glm::mat4(1), m_SquarePosition)
		//);


		VoxelEngine::Renderer::EndScene();
	}
}
void GameLayer::OnEvent(VoxelEngine::Event& event) {
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

void GameLayer::OnImGuiRender() {
	//ImGui::Begin("Settings");
	//ImGui::End();
}

void GameLayer::OnDetach()
{
}
