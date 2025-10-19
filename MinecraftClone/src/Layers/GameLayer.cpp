#include "GameLayer.h"
#include "../Overlays/DebugOverlay.h"
#include <glm/gtc/matrix_transform.hpp>

struct FaceData {
	uint32_t packedPos;
};

struct FaceModel {
	const glm::vec2* texCoordsOrigin;
};
const int CHUNK_WIDTH = 16;
const int WORLD_WIDTH = 2;

struct Chunk {
	int x;
	int y;
	int z;
	uint32_t blockTypes[CHUNK_WIDTH][CHUNK_WIDTH][CHUNK_WIDTH];
};

struct ChunkQuads {
	uint32_t blockQuads[CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_WIDTH * 6];
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

	//TERRAIN
	//0 air 1 dirt
	//std::vector<uint32_t> terrainData;
	Chunk terrainData[WORLD_WIDTH * WORLD_WIDTH * WORLD_WIDTH];
	for (int x = 0; x < WORLD_WIDTH; x++) {
		for (int y = 0; y < WORLD_WIDTH; y++) {
			for (int z = 0; z < WORLD_WIDTH; z++) {
				int index = x + y * WORLD_WIDTH + z * WORLD_WIDTH * WORLD_WIDTH;
				// Fill each block in the chunk with 1
				for (int bx = 0; bx < CHUNK_WIDTH; bx++) {
					for (int by = 0; by < CHUNK_WIDTH; by++) {
						for (int bz = 0; bz < CHUNK_WIDTH; bz++) {
							terrainData[index].x = x * CHUNK_WIDTH;
							terrainData[index].y = y * CHUNK_WIDTH;
							terrainData[index].z = z * CHUNK_WIDTH;
							terrainData[index].blockTypes[bx][by][bz] = 1;
						}
					}
				}
			}
		}
	}

	uint32_t chunksSsbo;
	glCreateBuffers(1, &chunksSsbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, chunksSsbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, WORLD_WIDTH * WORLD_WIDTH * WORLD_WIDTH * sizeof(Chunk), terrainData, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, chunksSsbo);

	//NEIGHBOURS
	//auto computeShader = m_ShaderLibrary.Load("assets/shaders/compute/getNeighbours.glsl");
	//computeShader->Bind();
	//glDispatchCompute(WORLD_WIDTH, WORLD_WIDTH, WORLD_WIDTH);
	//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	uint32_t debugSssbo;
	glCreateBuffers(1, &debugSssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, debugSssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 5 * 3 * 2 * 2 * 2 * sizeof(int), nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, debugSssbo);

	//GEN QUADS
	uint32_t genQuadsSsbo;
	glCreateBuffers(1, &genQuadsSsbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, genQuadsSsbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, WORLD_WIDTH * WORLD_WIDTH * WORLD_WIDTH * sizeof(ChunkQuads), nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, genQuadsSsbo);

	auto generateQuadsCompute = m_ShaderLibrary.Load("assets/shaders/compute/generateQuads.glsl");
	generateQuadsCompute->Bind();
	glDispatchCompute(WORLD_WIDTH, WORLD_WIDTH, WORLD_WIDTH);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	m_ShaderLibrary.Load("assets/shaders/DrawTerrain.glsl");

	uint32_t quadInfo;
	glCreateBuffers(1, &quadInfo);
	const std::vector<glm::vec2>& subImagesCoordsList = m_TerrainAtlas->GetSubImagesCoordsList();
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, quadInfo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, subImagesCoordsList.size() * sizeof(glm::vec2), subImagesCoordsList.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, quadInfo);

	float textureOffset = 16.0f / m_TerrainAtlas->GetWidth();
	uint32_t textureOffsets;
	glCreateBuffers(1, &textureOffsets);
	glm::vec2 textureOffsetsData[] = {
		{0.0,  0.0},
		{textureOffset,  0.0},
		{textureOffset,  textureOffset},
		{0.0, textureOffset},
	};
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, textureOffsets);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(textureOffsetsData), textureOffsetsData, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, textureOffsets);

	m_ShaderLibrary.Load("assets/shaders/compute/debug.glsl");

	int vertsPerQuad = 6;
	int quadsPerBlock = 6;
	int blocks = 16 * 16 * 16;
	int chunks = 2 * 2 * 2;
	int vertsPerChunk = blocks * quadsPerBlock * vertsPerQuad;
	for (int i = 0; i < chunks; i++) {
		m_Cmd[i].count = vertsPerChunk;
		m_Cmd[i].instanceCount = 1;
		m_Cmd[i].first = 0;
		m_Cmd[i].baseInstance = i;
	}
	GLuint indirectBuffer;
	glGenBuffers(1, &indirectBuffer);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(m_Cmd), m_Cmd, GL_STATIC_DRAW);
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

		auto drawTerrainShader = m_ShaderLibrary.Get("DrawTerrain");
		m_TerrainAtlas->Bind();
		//VoxelEngine::Renderer::Submit(ssboShader, m_SsboVao,
		//	glm::translate(glm::mat4(1), glm::vec3(0, 0, -1))
		//);
		//glDrawArrays(GL_TRIANGLES, 0, 6 * 6);

		VoxelEngine::Renderer::Submit(drawTerrainShader,
			glm::translate(glm::mat4(1), glm::vec3(0, 0, -1))
		);

		glMultiDrawArraysIndirect(GL_TRIANGLES, 0, 2 * 2 * 2, 0);
		//glDrawArrays(GL_TRIANGLES, 0, chunks * blocks * quadsPerBlock * vertsPerQuad);

		auto debugCompute = m_ShaderLibrary.Get("debug");
		debugCompute->Bind();
		glDispatchCompute(WORLD_WIDTH, WORLD_WIDTH, WORLD_WIDTH);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
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
