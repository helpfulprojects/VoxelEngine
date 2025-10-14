#pragma once
#include <VoxelEngine.h>
class GameLayer : public VoxelEngine::Layer {
public:
	GameLayer();
	~GameLayer();
	void OnAttach() override;
	void OnUpdate(VoxelEngine::Timestep ts) override;
	void OnEvent(VoxelEngine::Event& event) override;
	virtual void OnImGuiRender() override;
	virtual void OnDetach() override;

	inline VoxelEngine::PerspectiveCamera& GetCamera() { return m_Camera; }
private:
	VoxelEngine::ShaderLibrary m_ShaderLibrary;
	VoxelEngine::Ref<VoxelEngine::VertexArray> m_SquareVA;
	VoxelEngine::Ref<VoxelEngine::Texture2D> m_Texture, m_ChernoLogoTexture;
	VoxelEngine::Ref<VoxelEngine::TextureAtlas> m_TerrainAtlas;
	VoxelEngine::PerspectiveCamera m_Camera;
	glm::vec3 m_SquarePosition;
	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 5.0f;
	VoxelEngine::Ref<VoxelEngine::VertexArray> m_SsboVao;
};
