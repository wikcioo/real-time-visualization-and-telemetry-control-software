#pragma once

#include "camera.hpp"

class FpvCamera : public Camera
{
public:
    FpvCamera(const glm::vec3& position, const glm::vec3& direction, float aspectRatio);

    void SetAspectRatio(float aspectRatio);
    void ProcessKeyPress(float dt);
    void ProcessMouseMovement(float xChange, float yChange, float dt);
private:
    float m_Pitch;
    float m_Yaw;

    float m_AspectRatio;
    float m_MovementSpeed;
private:
    void RecalculateMatrices();
};
