#pragma once
#include <VoxelEngine/Core/Core.h>
#include <glm/glm.hpp>
namespace VoxelEngine {
	class TextureSubImage2D {
	public:
		TextureSubImage2D(const std::string& path);
		~TextureSubImage2D();
		const std::string& GetName() const { return m_Name; };
		const texture_data* GetData() const { return m_Data; };
		const uint32_t GetWidth() const { return m_Width; };
		const uint32_t GetId() const { return m_Id; };
		const uint32_t GetHeight() const { return m_Height; };
		const glm::vec2& GetTexCoords() const { return m_TexCoords; };
		void SetId(uint32_t id) { m_Id = id; }
		void SetTexCoords(glm::vec2 texCoords) { m_TexCoords = texCoords; }
		void Colorize(glm::vec3& color);
		void FreeData();
	private:
		uint32_t m_Id, m_Width, m_Height, m_Channels;
		texture_data* m_Data;
		std::string m_Name;

		glm::vec2 m_TexCoords;
	};
}
