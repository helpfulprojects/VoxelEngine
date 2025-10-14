#include "pch.h"
#include "Texture.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"
#include "Platform/OpenGL/OpenGLTextureAtlas.h"
namespace VoxelEngine {
	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None: VE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(path);
		}
		VE_CORE_ASSERT(false, "Unkown RendererAPI!");
		return nullptr;
		return Ref<Texture2D>();
	}
	Ref<TextureAtlas> TextureAtlas::Create()
	{
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None: VE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTextureAtlas>();
		}
		VE_CORE_ASSERT(false, "Unkown RendererAPI!");
		return nullptr;
		return Ref<TextureAtlas>();
	}
	Ref<TextureSubImage2D> TextureAtlas::CreateTextureSubImage(const std::string& path)
	{
		return std::make_shared<TextureSubImage2D>(path);
	}
}
