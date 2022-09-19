#pragma once

#include <memory>
#include "shader.hpp"

class Material
{
public:
    Material();
    ~Material();

    void Initialize(std::shared_ptr<Shader> shader, float ambient, float diffuse, float specular);
    void ApplyUniforms();
private:
    std::shared_ptr<Shader> m_Shader;
    float m_Ambient;
    float m_Diffuse;
    float m_Specular;
};
