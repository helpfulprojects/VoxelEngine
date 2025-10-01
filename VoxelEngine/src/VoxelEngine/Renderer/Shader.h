#pragma once
#include <string>
#include <glm/glm.hpp>
namespace VoxelEngine {
	class Shader {
	public:
		Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
		~Shader();
		void Bind() const;
		void Unbind() const;

		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& values);
	private:
		uint32_t m_RendererID;
	};
}