#pragma once
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
typedef unsigned int GLenum;
namespace VoxelEngine {
	class Shader {
	public:
		Shader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		Shader(const std::string& filepath);
		Shader(const std::string& filepath, const std::string& globalDefines);
		~Shader();

		void Bind() const;
		void Unbind() const;
		const std::string& GetName() const {
			return m_Name;
		};
		void UploadUniformInt(const std::string& name, int value);

		void UploadUniformFloat(const std::string& name, float value);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& values);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& values);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& values);

		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);

		static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		static Ref<Shader> Create(const std::string& filePath);
		static Ref<Shader> Create(const std::string& filePath, const std::string& globalDefines);
	protected:
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);
	protected:
		uint32_t m_RendererID;
		std::string m_Name;
	};

	class ShaderLibrary {
	public:
		void Add(const Ref<Shader>& shader);
		void Add(const std::string& name, const Ref<Shader>& shader);
		Ref<Shader> Load(const std::string& filepath);
		Ref<Shader> Load(const std::string& filepath, const std::string& globalDefines);
		//Ref<Shader> Load(const std::string& name, const std::string& filepath);

		Ref<Shader> Get(const std::string& name);
		bool Exists(const std::string& name) const;
	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	};
}