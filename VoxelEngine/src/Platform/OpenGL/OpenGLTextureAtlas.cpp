#include "pch.h"
#include "OpenGLTextureAtlas.h"
#include <stb_image.h>
#include <glad/glad.h>
namespace VoxelEngine {
	OpenGLTextureAtlas::OpenGLTextureAtlas()
		: m_Width(0), m_Height(0), m_RendererID(0)
	{
	}
	OpenGLTextureAtlas::~OpenGLTextureAtlas()
	{
		VE_PROFILE_FUNCTION;
		if (m_RendererID != 0) {
			glDeleteTextures(1, &m_RendererID);
		}
	}
	void OpenGLTextureAtlas::Bind(uint32_t slot) const
	{
		VE_PROFILE_FUNCTION;
		glBindTextureUnit(slot, m_RendererID);
	}
	void OpenGLTextureAtlas::Add(const Ref<TextureSubImage2D>& textureSubImage)
	{
		auto& name = textureSubImage->GetName();
		VE_CORE_ASSERT(!Exists(name), "TextureSubImage2D already exists");
		m_SubTextures[name] = textureSubImage;
		m_SubTexturesBakeQueue.push(name);
	}
	bool OpenGLTextureAtlas::Exists(const std::string& name) const
	{
		return m_SubTextures.find(name) != m_SubTextures.end();
	}
	void OpenGLTextureAtlas::Bake()
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

		for (int y = 0; y < width; y++) {
			for (int x = 0; x < width; x++) {
				if (m_SubTexturesBakeQueue.empty()) {
					break;
				}
				std::string& subTextureKey = m_SubTexturesBakeQueue.front();
				Ref<TextureSubImage2D>& subTexture = m_SubTextures[subTextureKey];
				m_SubTexturesBakeQueue.pop();
				uint32_t xoffset = subTextureSize * x;
				uint32_t yoffset = m_Height - subTextureSize * (y + 1);
				glTextureSubImage2D(m_RendererID, 0, xoffset, yoffset, subTextureSize, subTextureSize, GL_RGBA, GL_UNSIGNED_BYTE, subTexture->GetData());
				subTexture->FreeData();
			}
		}
	}
}
