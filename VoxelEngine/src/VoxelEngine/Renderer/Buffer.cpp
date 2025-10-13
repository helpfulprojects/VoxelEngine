#include "pch.h"
#include "Buffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"
namespace VoxelEngine {
	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None: VE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return new OpenGLVertexBuffer(vertices, size);
		}
		VE_CORE_ASSERT(false, "Unkown RendererAPI!");
		return nullptr;
	}
	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t size)
	{
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None: VE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return new OpenGLIndexBuffer(indices, size);
		}
		VE_CORE_ASSERT(false, "Unkown RendererAPI!");
		return nullptr;
	}
	//StorageBuffer* StorageBuffer::Create()
	//{
	//	switch (Renderer::GetAPI()) {
	//	case RendererAPI::API::None: VE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	//	case RendererAPI::API::OpenGL: return new OpenGLStorageBuffer();
	//	}
	//	VE_CORE_ASSERT(false, "Unkown RendererAPI!");
	//	return nullptr;
	//}
}