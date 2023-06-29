#include "arcball_camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "core/input.hpp"

ArcballCamera::ArcballCamera(const glm::vec3& position, const glm::vec3& target, float aspectRatio)
    : Camera(position, glm::vec3(0)), m_Target(target)
{
    SetAspectRatio(aspectRatio);
    RecalculateMatrices();
}

void ArcballCamera::ProcessKeyPress(float dt)
{
    if (Input::IsKeyPressed(GLFW_KEY_R))
    {
        m_Target = glm::vec3(0);
    }

    RecalculateMatrices();
}

void ArcballCamera::ProcessMouseMovement(float xChange, float yChange, float dt)
{
    // TODO: Very hacky way of doing it, this method needs refactoring
    if (Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float old_distance = glm::length(m_Target - m_Position);
        m_Position -= m_Up * yChange * m_CameraSensitivity * 0.3f;
        m_Position -= m_Right * xChange * m_CameraSensitivity * 0.3f;
        float new_distance = glm::length(m_Target - m_Position);
        m_Position *= old_distance/new_distance;
    
        if (xChange != 0.0f || yChange != 0.0f)
            RecalculateMatrices();
    }
    else if (Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
    {
        float m = m_CameraSensitivity * 0.11f;
        m_Position -= m_Up * yChange * m;
        m_Position -= m_Right * xChange * m;
        m_Target -= m_Up * yChange * m;
        m_Target -= m_Right * xChange * m;

        if (xChange != 0.0f || yChange != 0.0f)
            RecalculateMatrices();
    }
}

void ArcballCamera::RecalculateMatrices()
{
    m_Direction = glm::normalize(m_Target - m_Position);
    m_Right     = glm::normalize(glm::cross(m_Direction, glm::vec3(0, 1, 0)));
    m_Up        = glm::normalize(glm::cross(m_Right, m_Direction));

    m_ViewMatrix = glm::lookAt(m_Position, m_Target, m_Up);
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}
