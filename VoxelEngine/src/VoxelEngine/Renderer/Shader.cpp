#include "pch.h"
#include "Shader.h"
#include "Renderer.h"
#include "VertexArray.h"
#include "VoxelEngine/Core/Utils.h"
#include <fstream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
namespace VoxelEngine {
static GLenum ShaderTypeFromString(const std::string &type) {
  if (type == "vertex")
    return GL_VERTEX_SHADER;
  if (type == "fragment")
    return GL_FRAGMENT_SHADER;
  if (type == "compute")
    return GL_COMPUTE_SHADER;
  if (type == "tessellationControl")
    return GL_TESS_CONTROL_SHADER;
  if (type == "tesselationEvaluation")
    return GL_TESS_EVALUATION_SHADER;
  if (type == "geometry")
    return GL_GEOMETRY_SHADER;

  VE_CORE_ASSERT(false, "Unknown shader type!");
  return 0;
}
Shader::Shader(const std::string &name, const std::string &vertexSrc,
               const std::string &fragmentSrc)
    : m_Name(name) {
  VE_PROFILE_FUNCTION;
  std::unordered_map<GLenum, std::string> sources;
  sources[GL_VERTEX_SHADER] = vertexSrc;
  sources[GL_FRAGMENT_SHADER] = fragmentSrc;
  Compile(sources);
}
Shader::Shader(const std::string &filepath) : m_Filepath(filepath) {
  VE_PROFILE_FUNCTION;
  std::string source = Utils::ReadFile(filepath);
  VE_CORE_ASSERT(!source.empty(), "Empty shader source read");
  auto shaderSources = PreProcess(source);
  Compile(shaderSources);

  m_Name = Utils::ExtractNameFromFilePath(filepath);
}
Shader::Shader(const std::string &filepath, const std::string &globalDefines)
    : m_Filepath(filepath) {
  VE_PROFILE_FUNCTION;
  std::string source = Utils::ReadFile(filepath);
  VE_CORE_ASSERT(!source.empty(), "Empty shader source read");
  std::unordered_map<GLenum, std::string> shaderSources = PreProcess(source);
  for (auto &&[key, value] : shaderSources) {
    GLenum type = key;
    if (type == GL_FRAGMENT_SHADER) {
      continue;
    }
    const std::string &source = value;
    shaderSources[key] = globalDefines + source;
  }
  Compile(shaderSources);

  m_Name = Utils::ExtractNameFromFilePath(filepath);
}
Shader::~Shader() {
  VE_PROFILE_FUNCTION;
  glDeleteProgram(m_RendererID);
}
void Shader::Bind() const {
  VE_PROFILE_FUNCTION;
  glUseProgram(m_RendererID);
}
void Shader::Unbind() const {
  VE_PROFILE_FUNCTION;
  glUseProgram(0);
}
void Shader::UploadUniformInt(const std::string &name, int value) {
  GLint location = glGetUniformLocation(m_RendererID, name.c_str());
  glUniform1i(location, value);
}
void Shader::UploadUniformFloat(const std::string &name, float value) {
  GLint location = glGetUniformLocation(m_RendererID, name.c_str());
  glUniform1f(location, value);
}
void Shader::UploadUniformFloat2(const std::string &name,
                                 const glm::vec2 &values) {
  GLint location = glGetUniformLocation(m_RendererID, name.c_str());
  glUniform2f(location, values.x, values.y);
}
void Shader::UploadUniformFloat3(const std::string &name,
                                 const glm::vec3 &values) {
  GLint location = glGetUniformLocation(m_RendererID, name.c_str());
  glUniform3f(location, values.x, values.y, values.z);
}
void Shader::UploadUniformMat3(const std::string &name,
                               const glm::mat3 &matrix) {
  GLint location = glGetUniformLocation(m_RendererID, name.c_str());
  glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}
void Shader::UploadUniformMat4(const std::string &name,
                               const glm::mat4 &matrix) {
  GLint location = glGetUniformLocation(m_RendererID, name.c_str());
  glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}
std::unordered_map<GLenum, std::string>
Shader::PreProcess(const std::string &source) {
  VE_PROFILE_FUNCTION;
  std::unordered_map<GLenum, std::string> shaderSources;

  const char *typeToken = "#type";
  size_t typeTokenLength = strlen(typeToken);
  size_t pos = source.find(typeToken, 0);
  while (pos != std::string::npos) {
    size_t eol = source.find_first_of("\r\n", pos);
    VE_CORE_ASSERT(eol != std::string::npos, "Syntax error");
    size_t begin = pos + typeTokenLength + 1;
    std::string type = source.substr(begin, eol - begin);
    VE_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified");

    size_t nextLinePos = source.find_first_not_of("\r\n", eol);
    pos = source.find(typeToken, nextLinePos);
    shaderSources[ShaderTypeFromString(type)] = source.substr(
        nextLinePos,
        (pos == std::string::npos ? source.size() : pos) - nextLinePos);
  }

  return shaderSources;
}
void Shader::Compile(
    const std::unordered_map<GLenum, std::string> &shaderSources) {
  VE_PROFILE_FUNCTION;
  GLuint program = glCreateProgram();
  VE_CORE_ASSERT(shaderSources.size() <= 5,
                 "We don't support that many shaders");
  std::array<GLenum, 5> glShaderIDs;
  int glShaderIDIndex = 0;
  for (auto &&[key, value] : shaderSources) {
    GLenum type = key;
    const std::string &source = value;

    GLuint shader = glCreateShader(type);
    const char *sourceCStr = source.c_str();
    glShaderSource(shader, 1, &sourceCStr, 0);
    {
      VE_PROFILE_SCOPE("Compile shader source code");
      glCompileShader(shader);
    }
    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
      GLint maxLength = 0;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

      // The maxLength includes the NULL character
      std::vector<GLchar> infoLog(maxLength);
      glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

      // We don't need the shader anymore.
      glDeleteShader(shader);

      VE_CORE_ERROR("{0} -> {1}", m_Filepath, infoLog.data());
      VE_CORE_ASSERT(false, "Shader compilation failure!");
      break;
    }
    glAttachShader(program, shader);
    glShaderIDs[glShaderIDIndex++] = shader;
  }

  {
    VE_PROFILE_SCOPE("Link shader program");
    // Link our m_RendererID
    glLinkProgram(program);
  }

  {
    VE_PROFILE_SCOPE("Check for shader linking errors");
    // Note the different functions here: glGetProgram* instead of glGetShader*.
    GLint isLinked = 0;
    {
      VE_PROFILE_SCOPE("Get link status");
      glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
    }
    if (isLinked == GL_FALSE) {
      GLint maxLength = 0;
      {
        VE_PROFILE_SCOPE("Get info log length");
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
      }

      // The maxLength includes the NULL character
      std::vector<GLchar> infoLog(maxLength);
      {
        VE_PROFILE_SCOPE("Get program info");
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
      }

      // We don't need the program anymore.
      {
        VE_PROFILE_SCOPE("Delete the program");
        glDeleteProgram(program);
      }
      // Don't leak shaders either.
      for (auto id : glShaderIDs) {
        glDeleteShader(id);
      }

      VE_CORE_ERROR("{0}", infoLog.data());
      VE_CORE_ASSERT(false, "Shader link failure!");
      return;
    }
  }

  for (auto id : glShaderIDs) {
    {
      VE_PROFILE_SCOPE("Detach shader");
      glDetachShader(program, id);
    }
  }
  m_RendererID = program;
}
void Shader::UploadUniformFloat4(const std::string &name,
                                 const glm::vec4 &values) {
  GLint location = glGetUniformLocation(m_RendererID, name.c_str());
  glUniform4f(location, values.x, values.y, values.z, values.w);
}

Ref<Shader> Shader::Create(const std::string &filePath) {
  return std::make_shared<Shader>(filePath);
}
Ref<Shader> Shader::Create(const std::string &filePath,
                           const std::string &globalDefines) {
  return std::make_shared<Shader>(filePath, globalDefines);
}
Ref<Shader> Shader::Create(const std::string &name,
                           const std::string &vertexSrc,
                           const std::string &fragmentSrc) {
  return std::make_shared<Shader>(name, vertexSrc, fragmentSrc);
}

// ------------------------------------------------------------

void ShaderLibrary::Add(const Ref<Shader> &shader) {
  auto &name = shader->GetName();
  Add(name, shader);
}
void ShaderLibrary::Add(const std::string &name, const Ref<Shader> &shader) {
  VE_CORE_ASSERT(!Exists(name), "Shader already exists");
  m_Shaders[name] = shader;
}
Ref<Shader> ShaderLibrary::Load(const std::string &filepath) {
  auto shader = Shader::Create(filepath);
  Add(shader);
  return shader;
}
Ref<Shader> ShaderLibrary::Load(const std::string &filepath,
                                const std::string &globalDefines) {
  auto shader = Shader::Create(filepath, globalDefines);
  Add(shader);
  return shader;
}
// Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string&
// filepath)
//{
//	auto shader = Shader::Create(filepath);
//	Add(name, shader);
//	return shader;
// }
Ref<Shader> ShaderLibrary::Get(const std::string &name) {
  VE_CORE_ASSERT(Exists(name), "Shader doesn't exists");
  return m_Shaders[name];
}
bool ShaderLibrary::Exists(const std::string &name) const {
  return m_Shaders.find(name) != m_Shaders.end();
}
} // namespace VoxelEngine
