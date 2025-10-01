#pragma once
#include "VoxelEngine/Renderer/Buffer.h"
#include <memory>
namespace VoxelEngine {
	class VertexArray {
	public:
		virtual ~VertexArray() {}
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) = 0;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) = 0;

		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
		virtual const Ref<IndexBuffer>& GetIndexBuffers() const = 0;

		static VertexArray* Create();
	};
}
