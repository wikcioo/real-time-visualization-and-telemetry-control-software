#include "camera.hpp"

PerspectiveCamera::PerspectiveCamera(const glm::vec3& position, const glm::vec3& direction, float aspectRatio)
    : m_ProjectionMatrix(1.0f), m_ViewMatrix(1.0f), m_Position(position), m_Front(glm::normalize(direction)), m_Right(1.0f), m_Up(1.0f),
      m_Pitch(0.0f), m_Yaw(-90.0f), m_AspectRatio(aspectRatio), m_MovementSpeed(DefaultMovementSpeed), m_CameraSensitivity(DefaultCameraSensitivity)
{
    SetAspectRatio(m_AspectRatio);
    UpdateViewMatrix();
}

void PerspectiveCamera::SetAspectRatio(float aspectRatio)
{
    m_AspectRatio = aspectRatio;
    m_ProjectionMatrix = glm::perspective(DefaultFov, m_AspectRatio, 0.1f, 100.0f);
}

void PerspectiveCamera::ProcessKeyboard(MovementDirection movementDirection, float dt)
{
    float velocity = dt * m_MovementSpeed;

    if (movementDirection == MovementDirection::FORWARD)
        m_Position += m_Front * velocity;
    if (movementDirection == MovementDirection::BACKWARD)
        m_Position -= m_Front * velocity;
    if (movementDirection == MovementDirection::LEFT)
        m_Position -= m_Right * velocity;
    if (movementDirection == MovementDirection::RIGHT)
        m_Position += m_Right * velocity;

    UpdateViewMatrix();
}

void PerspectiveCamera::ProcessMouseMovement(float xChange, float yChange)
{
    m_Pitch += yChange * m_CameraSensitivity;
    m_Yaw += xChange * m_CameraSensitivity;

    if (m_Pitch > 89.0f) m_Pitch = 89.9f;
    else if (m_Pitch < -89.0f) m_Pitch = -89.0f;

    UpdateViewMatrix();
}

void PerspectiveCamera::UpdateViewMatrix()
{
    glm::vec3 front;
    front.x = glm::cos(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));
    front.y = glm::sin(glm::radians(m_Pitch));
    front.z = glm::sin(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));

    m_Front = glm::normalize(front);
    m_Right = glm::normalize(glm::cross(m_Front, glm::vec3(0, 1, 0)));
    m_Up    = glm::normalize(glm::cross(m_Right, m_Front));

    m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}
