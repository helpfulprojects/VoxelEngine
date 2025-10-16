#include "pch.h"
#include "OpenGLRendererAPI.h"
#include <glad/glad.h>
#include <tracy/TracyOpenGL.hpp>
namespace VoxelEngine {
	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}
	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	void OpenGLRendererAPI::Init()
	{
		TracyGpuContext;
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glEnable(GL_CULL_FACE);
		//glEnable(GL_DEPTH_TEST);
	}
	void OpenGLRendererAPI::SetViewport(int x, int y, unsigned int width, unsigned int height)
	{
		glViewport(x, y, width, height);
	}
	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray)
	{
		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffers()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}
}
