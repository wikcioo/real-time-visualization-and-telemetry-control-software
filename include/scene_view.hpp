#pragma once
#include <map>
#include <memory>
#include "fpv_camera.hpp"
#include "entity.hpp"
#include "opengl_buffer_manager.hpp"

class SceneView
{
public:
    SceneView(int width, int height);
    ~SceneView();

    std::shared_ptr<Entity> GetEntity(const std::string& name) { return m_Entities.at(name); }
    void Update(float dt);
    void Draw();
private:
    float m_Width, m_Height;
    bool m_IsWindowHovered;
    std::pair<double, double> m_LastMousePosition;
    std::unique_ptr<FpvCamera> m_Camera;
    std::unique_ptr<renderer::GLFrameBuffer> m_Framebuffer;
    std::map<std::string, std::shared_ptr<Entity>> m_Entities;
private:
    void InitializeEntities();
};
