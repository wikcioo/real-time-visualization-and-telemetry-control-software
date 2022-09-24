#pragma once

#include "camera.hpp"

class ArcballCamera : public Camera
{
public:
    ArcballCamera(const glm::vec3& position, const glm::vec3& target, float aspectRatio);

    void ProcessKeyPress(float dt);
    void ProcessMouseMovement(float xChange, float yChange, float dt);
private:
    glm::vec3 m_Target;
private:
    void RecalculateMatrices();
};
