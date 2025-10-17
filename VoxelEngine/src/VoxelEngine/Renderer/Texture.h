#pragma once
#include <string>
#include <VoxelEngine/Core/Core.h>
#include "TextureSubImage2D.h"
#include <queue>
namespace VoxelEngine {
	class Texture {
	public:
		virtual ~Texture() = default;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual void Bind(uint32_t slot = 0) const = 0;
	};

	class Texture2D : public Texture {
	public:
		Texture2D(const std::string& path);
		virtual ~Texture2D() override;
		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual void Bind(uint32_t slot = 0) const override;
		static Ref<Texture2D> Create(const std::string& path);
	private:
		std::string m_Path;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
	};
	//Should be able to call subtexture just by its file name
	class TextureAtlas : public Texture {
	public:
		TextureAtlas();
		virtual ~TextureAtlas() override;
		void Add(const Ref<TextureSubImage2D>& textureSubImage);
		bool Exists(const std::string& name) const;
		uint32_t GetSubImageId(const std::string& name)const;
		const std::vector<glm::vec2>& GetSubImagesCoordsList()const { return m_SubImagesCoordsList; };
		void Bake();
		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual void Bind(uint32_t slot = 0) const override;
		static Ref<TextureAtlas> Create();
		static Ref<TextureSubImage2D> CreateTextureSubImage(const std::string& path);
	private:
		std::string m_Path;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;

		std::unordered_map<std::string, Ref<TextureSubImage2D>> m_SubTextures;
		std::queue<std::string> m_SubTexturesBakeQueue;
		std::vector<glm::vec2> m_SubImagesCoordsList;
	};
}
