#pragma once
#include <VoxelEngine.h>
#include <imgui.h>
class DebugOverlay : public VoxelEngine::Layer {
public:
    DebugOverlay();

    ~DebugOverlay();
    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(VoxelEngine::Timestep ts) override;
    virtual void OnImGuiRender() override;
    virtual void OnEvent(VoxelEngine::Event& event) override;

private:
    bool m_Visible = false;
    // VoxelEngine::OrthographicCamera m_Camera;
};
