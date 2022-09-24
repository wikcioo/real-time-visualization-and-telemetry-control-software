#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "mesh.hpp"
#include "material.hpp"

class Entity
{
public:
    static unsigned int id;
    Entity();
    ~Entity();

    void Initialize(std::vector<Vertex> vertices, std::vector<unsigned int> indices, unsigned int mode,
            const char *vs_filename, const char *fs_filename,
            const glm::mat4& translation = glm::mat4(1.0f), const glm::mat4& rotation = glm::mat4(1.0f), const glm::mat4& scale = glm::mat4(1.0f));
    void SetTranslation(const glm::mat4& translation);
    void SetRotation(const glm::mat4& rotation);
    void SetScale(const glm::mat4& scale);
    void SetColor(const glm::vec3& color);

    void SetNewShader(const char *vs_filename, const char *fs_filename);
    unsigned int GetID() { return m_ID; }

    void Draw(const glm::mat4& viewProjectionMatrix);
private:
    std::unique_ptr<Mesh> m_Mesh;
    std::unique_ptr<Material> m_Material;
    std::shared_ptr<Shader> m_Shader;
    glm::mat4 m_Translation;
    glm::mat4 m_Rotation;
    glm::mat4 m_Scale;
    glm::mat4 m_Model;
    unsigned int m_ID;
private:
    void RecalculateModelMatrix();
};
