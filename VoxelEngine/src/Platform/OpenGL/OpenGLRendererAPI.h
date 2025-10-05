#pragma once
#include "VoxelEngine/Renderer/RendererAPI.h"
namespace VoxelEngine {
	class OpenGLRendererAPI : public RendererAPI {
	public:
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;
		virtual void Init() override;
		virtual void SetViewport(int x, int y, unsigned int width, unsigned int height) override;
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) override;
	private:
	};
}
