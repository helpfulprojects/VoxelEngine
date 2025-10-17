#include "pch.h"
#include "Buffer.h"

#include <glad/glad.h>
#include "Renderer.h"
#include <tracy/TracyOpenGL.hpp>
namespace VoxelEngine {
	///////////////////////////////////////////////////////////////////////////////////////////	
	// VertexBuffer ///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////	
	VertexBuffer::VertexBuffer(float* vertices, uint32_t size) {
		VE_PROFILE_FUNCTION;
		TracyGpuZone("Create vertex buffer");
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}
	VertexBuffer::~VertexBuffer()
	{
		VE_PROFILE_FUNCTION;
		glDeleteBuffers(1, &m_RendererID);
	}
	void VertexBuffer::Bind() const
	{
		VE_PROFILE_FUNCTION;
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}
	void VertexBuffer::Unbind() const
	{
		VE_PROFILE_FUNCTION;
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
	{
		return new VertexBuffer(vertices, size);
	}
	///////////////////////////////////////////////////////////////////////////////////////////	
	// IndexBuffer ////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////	
	IndexBuffer::IndexBuffer(uint32_t* indices, uint32_t count) :m_Count(count)
	{
		VE_PROFILE_FUNCTION;
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}
	IndexBuffer::~IndexBuffer()
	{
		VE_PROFILE_FUNCTION;
		glDeleteBuffers(1, &m_RendererID);
	}
	void IndexBuffer::Bind() const
	{
		VE_PROFILE_FUNCTION;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}
	void IndexBuffer::Unbind() const
	{
		VE_PROFILE_FUNCTION;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t size)
	{
		return new IndexBuffer(indices, size);
	}
}