#pragma once
#include <imgui.h>
#include "VoxelEngine/Core/Layer.h"
namespace VoxelEngine {
class VE_API ImGuiLayer : public Layer {
public:
  ImGuiLayer();
  ~ImGuiLayer();
  virtual void OnAttach() override;
  virtual void OnDetach() override;
  virtual void OnImGuiRender() override {};
  void Begin();
  void End();

private:
  float m_Time = 0.0f;
};
} // namespace VoxelEngine
