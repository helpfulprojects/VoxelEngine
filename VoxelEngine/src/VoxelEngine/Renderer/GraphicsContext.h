#pragma once
struct GLFWwindow;
namespace VoxelEngine {
class GraphicsContext {
public:
    GraphicsContext(GLFWwindow* windowHandle);
    void Init();
    void SwapBuffers();

private:
    GLFWwindow* m_WindowHandle;
};
} // namespace VoxelEngine