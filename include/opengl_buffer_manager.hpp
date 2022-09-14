#pragma once

#include <vector>
#include <glm/glm.hpp>

struct Vertex
{
    glm::vec3 Position;
};

class OpenGLBufferManager
{
public:
    OpenGLBufferManager();
    ~OpenGLBufferManager();

    void CreateBuffers(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    void DeleteBuffers();

    void Bind();
    void Unbind();

    void Draw();
private:
    unsigned int m_VAO, m_VBO, m_EBO, m_IndexCount;
};
