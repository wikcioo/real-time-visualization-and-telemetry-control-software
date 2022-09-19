#include "entity.hpp"

#include <glm/gtc/matrix_transform.hpp>

Entity::Entity()
{
}

Entity::~Entity()
{
    m_Mesh->DeleteBuffers();
}

void Entity::Initialize(
        std::vector<Vertex> vertices, std::vector<unsigned int> indices,
        const char *vs_filename, const char *fs_filename,
        const glm::mat4& translation, const glm::mat4& rotation, const glm::mat4& scale
)
{
    m_Shader = std::make_shared<Shader>(vs_filename, fs_filename);

    m_Mesh = std::make_unique<Mesh>();
    m_Mesh->Initialize(vertices, indices);

    m_Material = std::make_unique<Material>();
    m_Material->Initialize(m_Shader, 0.3f, 0.5f, 1.0f);

    SetTranslation(translation);
    SetRotation(rotation);
    SetScale(scale);

    RecalculateModelMatrix();
}

void Entity::SetTranslation(const glm::mat4& translation)
{
    m_Translation = translation;
    RecalculateModelMatrix();
}

void Entity::SetRotation(const glm::mat4& rotation)
{
    m_Rotation = rotation;
    RecalculateModelMatrix();
}

void Entity::SetScale(const glm::mat4& scale)
{
    m_Scale = scale;
    RecalculateModelMatrix();
}

void Entity::RecalculateModelMatrix()
{
    m_Model = m_Translation * m_Rotation * m_Scale;
}

void Entity::SetNewShader(const char *vs_filename, const char *fs_filename)
{
    m_Shader->HotReloadProgram(vs_filename, fs_filename);
}

void Entity::Draw(const glm::mat4& viewProjectionMatrix)
{
    m_Shader->Bind();
    m_Shader->SetMat4("u_ViewProjection", viewProjectionMatrix);
    m_Shader->SetMat4("u_Model", m_Model);
    // m_Material->ApplyUniforms();

    m_Mesh->Bind();
    m_Mesh->Draw();
    m_Mesh->Unbind();
}

