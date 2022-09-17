#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class MovementDirection
{
    FORWARD, BACKWARD, LEFT, RIGHT
};

static constexpr float DefaultFov = 45.0f;
static constexpr float DefaultMovementSpeed = 3.0f;
static constexpr float DefaultCameraSensitivity = 0.05f;

class PerspectiveCamera
{
public:
    PerspectiveCamera(const glm::vec3& position, const glm::vec3& direction, float aspectRatio);

    void SetAspectRatio(float aspectRatio);
    void ProcessKeyboard(MovementDirection movementDirection, float dt);
    void ProcessMouseMovement(float xChange, float yChange);

    const glm::mat4& GetViewProjection() const { return m_ViewProjectionMatrix; }
private:
    glm::mat4 m_ProjectionMatrix;
    glm::mat4 m_ViewMatrix;
    glm::mat4 m_ViewProjectionMatrix;

    glm::vec3 m_Position;
    glm::vec3 m_Front;
    glm::vec3 m_Right;
    glm::vec3 m_Up;

    float m_Pitch;
    float m_Yaw;

    float m_AspectRatio;
    float m_MovementSpeed;
    float m_CameraSensitivity;
private:
    void UpdateViewMatrix();
};
