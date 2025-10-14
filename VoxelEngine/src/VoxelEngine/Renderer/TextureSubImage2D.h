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
		void FreeData();
	private:
		uint32_t m_Id, m_Width, m_Height, m_Channels;
		texture_data* m_Data;
		std::string m_Name;

		glm::vec2 m_TexCoords;
	};
}
