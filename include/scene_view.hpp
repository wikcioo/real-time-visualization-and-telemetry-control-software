#pragma once
#include <map>
#include <memory>
#include "fpv_camera.hpp"
#include "arcball_camera.hpp"
#include "entity.hpp"
#include "opengl_buffer_manager.hpp"

enum class CameraType
{
    ARCBALL, FPV
};

class SceneView
{
public:
    SceneView(int width, int height, CameraType cameraType);
    ~SceneView();

    std::shared_ptr<Entity> GetEntity(const std::string& name);
    const std::map<std::string, std::shared_ptr<Entity>>& GetEntities() { return m_Entities; }
    const CameraType& GetCameraType() { return m_CameraType; }
    bool IsSceneInteractive() { return m_IsSceneInteractive; }
    void SetSceneInteractive(bool interactive);
    void SetCameraType(const CameraType& type);
    void PrintEntities();
    void Update(float dt);
    void Draw();
private:
    float m_Width, m_Height;
    bool m_IsSceneInteractive;
    bool m_MouseFirstEnter;
    std::pair<double, double> m_LastMousePosition;
    std::unique_ptr<Camera> m_Camera;
    CameraType m_CameraType;
    std::unique_ptr<renderer::GLFrameBuffer> m_Framebuffer;
    std::map<std::string, std::shared_ptr<Entity>> m_Entities;
private:
    void InitializeEntities();
};
