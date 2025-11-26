#include "pch.h"
#include "Buffer.h"

#include <glad/glad.h>
#include "Renderer.h"
namespace VoxelEngine {
///////////////////////////////////////////////////////////////////////////////////////////
// VertexBuffer
// ///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
VertexBuffer::VertexBuffer(float* vertices, uint32_t size)
{
    VE_PROFILE_FUNCTION;
    glCreateBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_DYNAMIC_DRAW);
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
// IndexBuffer
// ////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
IndexBuffer::IndexBuffer(uint32_t* indices, uint32_t count)
    : m_Count(count)
{
    VE_PROFILE_FUNCTION;
    glCreateBuffers(1, &m_RendererID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices,
        GL_STATIC_DRAW);
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

///////////////////////////////////////////////////////////////////////////////////////////
// Shader Storage Buffer
// ////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

StorageBuffer::StorageBuffer(uint32_t target, const void* data, uint32_t size,
    uint32_t flags)
    : m_Target(target)
{
    VE_PROFILE_FUNCTION;
    glCreateBuffers(1, &m_RendererID);
    glNamedBufferStorage(m_RendererID, size, data, flags);
}

StorageBuffer::~StorageBuffer()
{
    VE_PROFILE_FUNCTION;
    glDeleteBuffers(1, &m_RendererID);
}

void StorageBuffer::Bind() const
{
    glBindBuffer(m_Target, m_RendererID);
}
void StorageBuffer::Unbind() const
{
    glBindBuffer(m_Target, 0);
}
void StorageBuffer::BindBufferBase(uint32_t index)
{
    VE_PROFILE_FUNCTION;
    glBindBufferBase(m_Target, index, m_RendererID);
}

void* StorageBuffer::MapBufferRange(uint32_t offset, uint32_t length,
    uint32_t access)
{
    return glMapNamedBufferRange(m_RendererID, offset, length, access);
}

Ref<StorageBuffer> StorageBuffer::Create(uint32_t target, const void* data, uint32_t size,
    uint32_t flags)
{

    return std::make_shared<StorageBuffer>(target, data, size, flags);
}

} // namespace VoxelEngine