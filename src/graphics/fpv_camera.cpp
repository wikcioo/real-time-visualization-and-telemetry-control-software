#include <glm/gtc/matrix_transform.hpp>

#include "core/input.hpp"
#include "fpv_camera.hpp"

FpvCamera::FpvCamera(const glm::vec3& position, const glm::vec3& direction, float aspectRatio)
    : Camera(position, direction), m_Pitch(0.0f), m_Yaw(-90.0f), m_MovementSpeed(s_DefaultMovementSpeed)
{
    SetAspectRatio(m_AspectRatio);
    RecalculateMatrices();
}

void FpvCamera::ProcessKeyPress(float dt)
{
    bool keyPressed = false;
    float velocity = dt * m_MovementSpeed;

    // TODO: Change from hardcoded movement direction keys(WASD) to generic(any)
    if (Input::IsKeyPressed(GLFW_KEY_W))
    {
        m_Position += m_Direction * velocity;
        keyPressed = true;
    }
    if (Input::IsKeyPressed(GLFW_KEY_S))
    {
        m_Position -= m_Direction * velocity;
        keyPressed = true;
    }
    if (Input::IsKeyPressed(GLFW_KEY_A))
    {
        m_Position -= m_Right * velocity;
        keyPressed = true;
    }
    if (Input::IsKeyPressed(GLFW_KEY_D))
    {
        m_Position += m_Right * velocity;
        keyPressed = true;
    }

    if (keyPressed)
        RecalculateMatrices();
}

void FpvCamera::ProcessMouseMovement(float xChange, float yChange, float dt)
{
    m_Pitch += yChange * m_CameraSensitivity;
    m_Yaw += xChange * m_CameraSensitivity;

    if (m_Pitch > 89.0f) m_Pitch = 89.9f;
    else if (m_Pitch < -89.0f) m_Pitch = -89.0f;

    if (xChange != 0.0f || yChange != 0.0f)
        RecalculateMatrices();
}

void FpvCamera::RecalculateMatrices()
{
    glm::vec3 dir;
    dir.x = glm::cos(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));
    dir.y = glm::sin(glm::radians(m_Pitch));
    dir.z = glm::sin(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));

    m_Direction = glm::normalize(dir);
    m_Right     = glm::normalize(glm::cross(m_Direction, glm::vec3(0, 1, 0)));
    m_Up        = glm::normalize(glm::cross(m_Right, m_Direction));

    m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Direction, m_Up);
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}
