#pragma once
#include "VoxelEngine/Renderer/VertexArray.h"
namespace VoxelEngine {
	class OpenGLVertexArray : public VertexArray {
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();
		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;

		inline virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const override { return m_VertexBuffers; };
		inline virtual const Ref<IndexBuffer>& GetIndexBuffers() const override { return m_IndexBuffer; };
	private:
		uint32_t m_RendererID;
		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
		Ref<IndexBuffer> m_IndexBuffer;
	};
}
