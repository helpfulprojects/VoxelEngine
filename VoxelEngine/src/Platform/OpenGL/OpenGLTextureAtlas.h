#pragma once
#include "VoxelEngine/Renderer/Texture.h"
#include <VoxelEngine/Renderer/TextureSubImage2D.h>
#include <queue>
namespace VoxelEngine {
	class OpenGLTextureAtlas : public TextureAtlas {
	public:
		OpenGLTextureAtlas();
		virtual ~OpenGLTextureAtlas() override;
		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual void Bind(uint32_t slot = 0) const override;
		virtual uint32_t GetSubImageId(const std::string& name)const override;
		virtual const std::vector<glm::vec2>& GetSubImagesCoordsList()const override { return m_SubImagesCoordsList; };

		virtual void Add(const Ref<TextureSubImage2D>& textureSubImage) override;
		virtual bool Exists(const std::string& name) const override;
		virtual void Bake() override;
	private:
		std::string m_Path;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;

		std::unordered_map<std::string, Ref<TextureSubImage2D>> m_SubTextures;
		std::queue<std::string> m_SubTexturesBakeQueue;
		std::vector<glm::vec2> m_SubImagesCoordsList;
	};
}

