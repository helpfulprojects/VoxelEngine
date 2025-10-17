#include "pch.h"
#include "Texture.h"
#include "Renderer.h"
#include <stb_image.h>
#include <glad/glad.h>
namespace VoxelEngine {
	Texture2D::Texture2D(const std::string& path)
		:m_Path(path)
	{
		VE_PROFILE_FUNCTION;
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = nullptr;
		{

			VE_PROFILE_SCOPE("stbi_load - Texture2D::Texture2D(const std::string&))");
			data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		}
		VE_CORE_ASSERT(data, "Failed to load image!");
		m_Width = width;
		m_Height = height;

		GLenum internalFormat = 0, dataFormat = 0;

		if (channels == 4) {
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3) {
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}

		VE_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}
	Texture2D::~Texture2D()
	{
		VE_PROFILE_FUNCTION;
		glDeleteTextures(1, &m_RendererID);
	}
	void Texture2D::Bind(uint32_t slot) const
	{
		VE_PROFILE_FUNCTION;
		glBindTextureUnit(slot, m_RendererID);
	}
	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		return std::make_shared<Texture2D>(path);
	}

	///////////////////////////////////////////////////////////////////////////

	TextureAtlas::TextureAtlas()
		: m_Width(0), m_Height(0), m_RendererID(0)
	{
	}
	TextureAtlas::~TextureAtlas()
	{
		VE_PROFILE_FUNCTION;
		if (m_RendererID != 0) {
			glDeleteTextures(1, &m_RendererID);
		}
	}
	void TextureAtlas::Bind(uint32_t slot) const
	{
		VE_PROFILE_FUNCTION;
		glBindTextureUnit(slot, m_RendererID);
	}
	uint32_t TextureAtlas::GetSubImageId(const std::string& name) const
	{
		auto subTexture = m_SubTextures.find(name);
		if (subTexture == m_SubTextures.end()) {
			return 0;
		}
		else {
			return subTexture->second->GetId();
		}
	}
	void TextureAtlas::Add(const Ref<TextureSubImage2D>& textureSubImage)
	{
		auto& name = textureSubImage->GetName();
		VE_CORE_ASSERT(!Exists(name), "TextureSubImage2D already exists");
		m_SubTextures[name] = textureSubImage;
		m_SubTexturesBakeQueue.push(name);
	}
	bool TextureAtlas::Exists(const std::string& name) const
	{
		return m_SubTextures.find(name) != m_SubTextures.end();
	}
	void TextureAtlas::Bake()
	{
		int subTexturesCount = m_SubTextures.size();
		int width = std::ceil(std::sqrt(subTexturesCount));
		const uint32_t subTextureSize = 16;
		m_Width = subTextureSize * width;
		m_Height = subTextureSize * width;
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, GL_RGBA8, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		uint32_t id = 0;
		for (int y = 0; y < width; y++) {
			for (int x = 0; x < width; x++) {
				if (m_SubTexturesBakeQueue.empty()) {
					break;
				}
				std::string& subTextureKey = m_SubTexturesBakeQueue.front();
				Ref<TextureSubImage2D>& subTexture = m_SubTextures[subTextureKey];
				m_SubTexturesBakeQueue.pop();
				uint32_t xoffset = subTextureSize * x;
				uint32_t yoffset = subTextureSize * y;
				subTexture->SetId(id++);
				//subTexture->SetTexCoords({ xoffset / m_Width,yoffset / m_Height });
				m_SubImagesCoordsList.push_back({ (float)xoffset / m_Width,(float)yoffset / m_Height });
				glTextureSubImage2D(m_RendererID, 0, xoffset, yoffset, subTexture->GetWidth(), subTexture->GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, subTexture->GetData());
				subTexture->FreeData();
			}
		}
	}
	Ref<TextureAtlas> TextureAtlas::Create()
	{
		return std::make_shared<TextureAtlas>();
	}
	Ref<TextureSubImage2D> TextureAtlas::CreateTextureSubImage(const std::string& path)
	{
		return std::make_shared<TextureSubImage2D>(path);
	}
}
