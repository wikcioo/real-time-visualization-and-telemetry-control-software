#include "material.hpp"


Material::Material()
{
}

Material::~Material()
{
}

void Material::Initialize(std::shared_ptr<Shader> shader, float ambient, float diffuse, float specular)
{
    m_Shader = shader;
    m_Ambient = ambient;
    m_Diffuse = diffuse;
    m_Specular = specular;
}

void Material::ApplyUniforms()
{
    m_Shader->SetFloat("u_Material.ambient", m_Ambient);
    m_Shader->SetFloat("u_Material.diffuse", m_Diffuse);
    m_Shader->SetFloat("u_Material.specular", m_Specular);
}
