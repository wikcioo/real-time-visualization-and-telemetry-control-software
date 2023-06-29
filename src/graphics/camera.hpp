#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

static constexpr float s_DefaultFov = 45.0f;
static constexpr float s_DefaultMovementSpeed = 3.0f;
static constexpr float s_DefaultCameraSensitivity = 0.05f;

class Camera
{
public:
    Camera(const glm::vec3& position, const glm::vec3& direction)
        : m_ProjectionMatrix(1.0f), m_ViewMatrix(1.0f), m_ViewProjectionMatrix(1.0f),
        m_Position(position), m_Direction(glm::normalize(direction)), m_Right(1, 0, 0), m_Up(0, 1, 0),
        m_CameraSensitivity(s_DefaultCameraSensitivity)
    {
    }

    virtual ~Camera() {}

    virtual void SetAspectRatio(float aspectRatio)
    {
        m_AspectRatio = aspectRatio;
        m_ProjectionMatrix = glm::perspective(glm::radians(s_DefaultFov), m_AspectRatio, 0.1f, 100.0f);
        RecalculateMatrices();
    }

    virtual void ProcessKeyPress(float dt) = 0;
    virtual void ProcessMouseMovement(float xChange, float yChange, float dt) = 0;

    virtual const glm::mat4& GetViewProjectionMatrix() const
    {
        return m_ViewProjectionMatrix;
    }
protected:
    glm::mat4 m_ProjectionMatrix;
    glm::mat4 m_ViewMatrix;
    glm::mat4 m_ViewProjectionMatrix;

    glm::vec3 m_Position;
    glm::vec3 m_Direction;
    glm::vec3 m_Right;
    glm::vec3 m_Up;

    float m_AspectRatio;
    float m_CameraSensitivity;
protected:
    virtual void RecalculateMatrices() = 0;
};
