#include "GameLayer.h"
#include "../Overlays/DebugOverlay.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

struct FaceData {
	uint32_t packedPos;
};

struct FaceModel {
	const glm::vec2* texCoordsOrigin;
};

GameLayer::GameLayer()
	:Layer("Example"),
	m_Camera(70.0f, 0.1f, 1500.0f),
	m_SquarePosition(-1.1f, 0, -0.5f),
	m_CameraPosition(0, 0, 0)
{
	m_TerrainAtlas = VoxelEngine::TextureAtlas::Create();
	VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> dirt = VoxelEngine::TextureAtlas::CreateTextureSubImage("assets/textures/texture_pack/assets/minecraft/textures/block/dirt.png");
	VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> tnt_side = VoxelEngine::TextureAtlas::CreateTextureSubImage("assets/textures/texture_pack/assets/minecraft/textures/block/tnt_side.png");
	VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> tnt_bottom = VoxelEngine::TextureAtlas::CreateTextureSubImage("assets/textures/texture_pack/assets/minecraft/textures/block/tnt_bottom.png");
	VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> tnt_top = VoxelEngine::TextureAtlas::CreateTextureSubImage("assets/textures/texture_pack/assets/minecraft/textures/block/tnt_top.png");
	VoxelEngine::Ref<VoxelEngine::TextureSubImage2D> stone = VoxelEngine::TextureAtlas::CreateTextureSubImage("assets/textures/texture_pack/assets/minecraft/textures/block/stone.png");
	m_TerrainAtlas->Add(dirt);
	m_TerrainAtlas->Add(tnt_bottom);
	m_TerrainAtlas->Add(tnt_side);
	m_TerrainAtlas->Add(tnt_top);
	m_TerrainAtlas->Add(stone);
	m_TerrainAtlas->Bake();

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
	textureShader->UploadUniformInt("u_Texture", 0);

	//SSBO
	auto ssboShader = m_ShaderLibrary.Load("assets/shaders/Ssbo.glsl");
	uint32_t dirtBlockSsbo, ssbo, quadInfo, textureOffsets;
	glCreateBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	std::vector<FaceData> ssboVertices;

	m_SsboVao.reset(VoxelEngine::VertexArray::Create());
	m_SsboVao->Bind();
	for (int i = 0; i < 6; i++) {
		uint32_t normalId = i;
		glm::ivec3 position = glm::ivec3(0, 0, 0);
		uint32_t texId = 0;
		switch (i) {
		case 0:
			texId = m_TerrainAtlas->GetSubImageId("tnt_top");
			break;
		case 1:
			texId = m_TerrainAtlas->GetSubImageId("tnt_bottom");
			break;
		case 2:
			texId = m_TerrainAtlas->GetSubImageId("tnt_side");
			break;
		case 3:
			texId = m_TerrainAtlas->GetSubImageId("tnt_side");
			break;
		case 4:
			texId = m_TerrainAtlas->GetSubImageId("tnt_side");
			break;
		case 5:
			texId = m_TerrainAtlas->GetSubImageId("tnt_side");
			break;
		}
		uint32_t vertex = (position.x | position.y << 4 | position.z << 8 | normalId << 16 | texId << 19);
		ssboVertices.push_back({ vertex });
	}
	glBufferData(GL_SHADER_STORAGE_BUFFER, ssboVertices.size() * sizeof(FaceData), ssboVertices.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

	glCreateBuffers(1, &quadInfo);
	const std::vector<glm::vec2>& subImagesCoordsList = m_TerrainAtlas->GetSubImagesCoordsList();
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, quadInfo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, subImagesCoordsList.size() * sizeof(glm::vec2), subImagesCoordsList.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, quadInfo);

	float textureOffset = 16.0f / m_TerrainAtlas->GetWidth();
	glCreateBuffers(1, &textureOffsets);
	glm::vec2 textureOffsetsData[] = {
		{0.0,  0.0},
		{textureOffset,  0.0},
		{textureOffset,  textureOffset},
		{0.0, textureOffset},
	};
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, textureOffsets);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(textureOffsetsData), textureOffsetsData, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, textureOffsets);


	glCreateBuffers(1, &dirtBlockSsbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, dirtBlockSsbo);
	std::vector<FaceData> dirtBlockVerts;

	m_DirtBlock.reset(VoxelEngine::VertexArray::Create());
	m_DirtBlock->Bind();
	for (int i = 0; i < 6; i++) {
		uint32_t normalId = i;
		glm::ivec3 position = glm::ivec3(2, 0, 0);
		uint32_t texId = m_TerrainAtlas->GetSubImageId("dirt");
		uint32_t vertex = (position.x | position.y << 4 | position.z << 8 | normalId << 16 | texId << 19);
		dirtBlockVerts.push_back({ vertex });
	}
	glBufferData(GL_SHADER_STORAGE_BUFFER, dirtBlockVerts.size() * sizeof(FaceData), dirtBlockVerts.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, dirtBlockSsbo);
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

		auto textureShader = m_ShaderLibrary.Get("Texture");
		//m_Texture->Bind();

		auto ssboShader = m_ShaderLibrary.Get("Ssbo");
		m_TerrainAtlas->Bind();
		VoxelEngine::Renderer::Submit(ssboShader, m_SsboVao,
			glm::translate(glm::mat4(1), glm::vec3(0, 0, -1))
		);
		glDrawArrays(GL_TRIANGLES, 0, 6 * 6);

		VoxelEngine::Renderer::Submit(ssboShader, m_DirtBlock,
			glm::translate(glm::mat4(1), glm::vec3(0, 0, -1))
		);
		glDrawArrays(GL_TRIANGLES, 0, 6 * 6);
		//glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, 6 * 6, 1, 22);

		//m_ChernoLogoTexture->Bind();
		//m_TerrainAtlas->Bind();
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
