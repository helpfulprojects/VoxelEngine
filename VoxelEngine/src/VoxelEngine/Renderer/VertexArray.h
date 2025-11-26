#pragma once
#include "VoxelEngine/Renderer/Buffer.h"
#include <memory>
namespace VoxelEngine {
class VertexArray {
public:
    VertexArray();
    ~VertexArray();
    void Bind() const;
    void Unbind() const;
    void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer);
    void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer);

    inline const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const
    {
        return m_VertexBuffers;
    };
    inline const Ref<IndexBuffer>& GetIndexBuffers() const
    {
        return m_IndexBuffer;
    };
    static VertexArray* Create();

private:
    uint32_t m_RendererID;
    std::vector<Ref<VertexBuffer>> m_VertexBuffers;
    Ref<IndexBuffer> m_IndexBuffer;
};
} // namespace VoxelEngine
