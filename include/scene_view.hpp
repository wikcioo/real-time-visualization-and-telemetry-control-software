#pragma once
#include <map>
#include <memory>
#include "camera.hpp"
#include "entity.hpp"
#include "opengl_buffer_manager.hpp"

class SceneView
{
public:
    SceneView(int width, int height);
    ~SceneView();

    void ProcessMouseMovement(double xChange, double yChange, int button);

    std::shared_ptr<Entity> GetEntity(const std::string& name) { return m_Entities.at(name); }
    void SetSize(int width, int height);
    void Draw();
private:
    float m_Width, m_Height;
    std::unique_ptr<PerspectiveCamera> m_Camera;
    std::unique_ptr<renderer::GLFrameBuffer> m_Framebuffer;
    std::map<std::string, std::shared_ptr<Entity>> m_Entities;
private:
    void InitializeEntities();
};
