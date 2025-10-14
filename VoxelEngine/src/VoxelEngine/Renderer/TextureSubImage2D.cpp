#include "pch.h"
#include "TextureSubImage2D.h"
#include <stb_image.h>
#include "VoxelEngine/Core/Utils.h"
namespace VoxelEngine {
	TextureSubImage2D::TextureSubImage2D(const std::string& path)
	{
		VE_PROFILE_FUNCTION;
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = nullptr;
		{

			VE_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::string&))");
			data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		}
		VE_CORE_ASSERT(data, "Failed to load image!");
		VE_CORE_ASSERT(channels == 4 || channels == 3, "Format not supported!");
		m_Width = width;
		m_Height = height;
		m_Channels = channels;
		m_Data = data;
		m_Name = Utils::ExtractNameFromFilePath(path);
	}
	TextureSubImage2D::~TextureSubImage2D()
	{
		if (m_Data) {
			FreeData();
		}
	}
	void TextureSubImage2D::Colorize(glm::vec3& color)
	{
		color = color / 256.0f;
		int numPixels = m_Width * m_Height;
		for (int i = 0; i < numPixels; i++) {
			int idx = i * m_Channels;
			m_Data[idx + 0] = std::ceil(m_Data[idx + 0] * color.r);
			m_Data[idx + 1] = std::ceil(m_Data[idx + 1] * color.g);
			m_Data[idx + 2] = std::ceil(m_Data[idx + 2] * color.b);
		}
	}
	void TextureSubImage2D::FreeData()
	{
		stbi_image_free(m_Data);
		m_Data = nullptr;
	}
}
