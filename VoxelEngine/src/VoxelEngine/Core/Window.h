#pragma once
#include "pch.h"
#include "VoxelEngine/Core/Core.h"
#include "VoxelEngine/Events/Event.h"

namespace VoxelEngine {
struct WindowProps {
    std::string Title;
    unsigned int Width;
    unsigned int Height;
    WindowProps(const std::string& title = "Voxel Engine",
        unsigned int width = 1920, unsigned int height = 1080)
        : Title(title)
        , Width(width)
        , Height(height)
    {
    }
};

class VE_API Window {
public:
    using EventCallbackFn = std::function<void(Event&)>;
    virtual ~Window() { }
    virtual void OnUpdate() = 0;

    virtual unsigned int GetWidth() const = 0;
    virtual unsigned int GetHeight() const = 0;

    virtual float GetAspectRatio() const = 0;

    virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
    virtual void SetVSync(bool enabled) = 0;
    virtual void SetMaximized(bool enabled) = 0;
    virtual void SetCursorVisibility(bool visible) = 0;
    virtual void ToggleCursorVisibility() = 0;
    virtual bool IsVSync() const = 0;
    virtual double GetTime() const = 0;

    virtual void* GetNativeWindow() const = 0;

    static Window* Create(const WindowProps& props = WindowProps());
};
} // namespace VoxelEngine
