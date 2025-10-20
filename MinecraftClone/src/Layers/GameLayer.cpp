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
const int WORLD_WIDTH = 65;
const int WORLD_HEIGHT = 16;

struct Chunk {
	int x;
	int y;
	int z;
	int quadsCount;
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
	VE_PROFILE_FUNCTION;
	{
		VE_PROFILE_SCOPE("Bake texture atlas");
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

	}

	uint32_t chunksSsbo;
	{
		VE_PROFILE_SCOPE("Init chunks data ssbo");
		glCreateBuffers(1, &chunksSsbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, chunksSsbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, WORLD_WIDTH * WORLD_HEIGHT * WORLD_WIDTH * sizeof(Chunk), nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, chunksSsbo);
	}

	//uint32_t debugSssbo;
	//glCreateBuffers(1, &debugSssbo);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, debugSssbo);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int), nullptr, GL_DYNAMIC_DRAW);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, debugSssbo);


	{
		VE_PROFILE_SCOPE("Compute Shader: Generate blocks");
		//GEN BLOCKS
		auto generateBlocksCompute = m_ShaderLibrary.Load("assets/shaders/compute/generateBlocks.glsl");
		generateBlocksCompute->Bind();
		glDispatchCompute(WORLD_WIDTH, WORLD_HEIGHT, WORLD_WIDTH);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
	}

	uint32_t genQuadsSsbo;
	{
		VE_PROFILE_SCOPE("Init quads for render ssbo");
		//GEN QUADS
		glCreateBuffers(1, &genQuadsSsbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, genQuadsSsbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, WORLD_WIDTH * WORLD_HEIGHT * WORLD_WIDTH * sizeof(ChunkQuads), nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, genQuadsSsbo);
	}

	{
		VE_PROFILE_SCOPE("Compute Shader: Generate quads");
		auto generateQuadsCompute = m_ShaderLibrary.Load("assets/shaders/compute/generateQuads.glsl");
		generateQuadsCompute->Bind();
		glDispatchCompute(WORLD_WIDTH, WORLD_HEIGHT, WORLD_WIDTH);
		//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
	}
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

	{
		VE_PROFILE_SCOPE("Set up indirect buffer");
		int vertsPerQuad = 6;
		int quadsPerBlock = 6;
		int blocks = CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_WIDTH;
		int chunks = WORLD_WIDTH * WORLD_HEIGHT * WORLD_WIDTH;
		int vertsPerChunk = blocks * quadsPerBlock * vertsPerQuad;
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, chunksSsbo);
		Chunk* gpuData = (Chunk*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
		if (gpuData) {
			for (int i = 0; i < WORLD_WIDTH * WORLD_HEIGHT * WORLD_WIDTH; i++) {
				DrawArraysIndirectCommand cmd;
				cmd.count = gpuData[i].quadsCount * vertsPerQuad;
				cmd.instanceCount = 1;
				cmd.first = 0;
				cmd.baseInstance = i;
				m_Cmd.push_back(cmd);
			}

			// Always unmap after you’re done
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		}
		GLuint indirectBuffer;
		glGenBuffers(1, &indirectBuffer);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
		glBufferData(GL_DRAW_INDIRECT_BUFFER, m_Cmd.size() * sizeof(DrawArraysIndirectCommand), m_Cmd.data(), GL_STATIC_DRAW);
	}
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
		auto drawTerrainShader = m_ShaderLibrary.Get("DrawTerrain");
		m_TerrainAtlas->Bind();
		VoxelEngine::Renderer::Submit(drawTerrainShader,
			glm::translate(glm::mat4(1), glm::vec3(0, 0, -1))
		);
		{
			VE_PROFILE_SCOPE("MultiDrawArraysIndirect");
			glMultiDrawArraysIndirect(GL_TRIANGLES, 0, WORLD_WIDTH * WORLD_HEIGHT * WORLD_WIDTH, 0);
		}
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
