#pragma once
#include <string>
#include <VoxelEngine/Core/Core.h>
#include "TextureSubImage2D.h"
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
		static Ref<Texture2D> Create(const std::string& path);
	};
	//Should be able to call subtexture just by its file name
	class TextureAtlas : public Texture {
	public:
		virtual void Add(const Ref<TextureSubImage2D>& textureSubImage) = 0;
		virtual bool Exists(const std::string& name) const = 0;
		static Ref<TextureSubImage2D> CreateTextureSubImage(const std::string& path);
		virtual void Bake() = 0;
		static Ref<TextureAtlas> Create();
	};
}
