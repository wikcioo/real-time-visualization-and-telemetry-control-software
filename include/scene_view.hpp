#pragma once
#include <memory>
#include "camera.hpp"
#include "opengl_buffer_manager.hpp"

class SceneView
{
public:
    SceneView(int width, int height);
    ~SceneView();

    void ProcessMouseMovement(double xChange, double yChange, int button);

    void SetSize(int width, int height);
    void Draw();
private:
    float m_Width, m_Height;
    std::unique_ptr<PerspectiveCamera> m_Camera;
    std::unique_ptr<renderer::GLFrameBuffer> m_Framebuffer;
};
