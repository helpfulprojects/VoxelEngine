#include "pch.h"
#include "Shader.h"
#include "pch.h"
#include "VertexArray.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
namespace VoxelEngine {
	Shader* Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None: VE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return new OpenGLShader(vertexSrc, fragmentSrc);
		}
		VE_CORE_ASSERT(false, "Unkown RendererAPI!");
		return nullptr;
	}
}
