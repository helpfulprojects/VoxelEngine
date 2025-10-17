#pragma once
#include <glm/glm.hpp>
#include "VoxelEngine/Renderer/VertexArray.h"
#include <memory>
namespace VoxelEngine {
	class RendererAPI {
	public:
		void SetClearColor(const glm::vec4& color);
		void Clear();
		void Init();
		void SetViewport(int x, int y, unsigned int width, unsigned int height);
		void DrawIndexed(const Ref<VertexArray>& vertexArray);
	};
}
