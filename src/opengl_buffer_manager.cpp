#include "../include/opengl_buffer_manager.hpp"

#include <GL/glew.h>

OpenGLBufferManager::OpenGLBufferManager()
    : m_VAO(0), m_VBO(0), m_EBO(0), m_IndexCount(0)
{
}

OpenGLBufferManager::~OpenGLBufferManager()
{
    DeleteBuffers();
}

void OpenGLBufferManager::CreateBuffers(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
{
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*) 0);
    // TODO: Add more attributes like Normals and Texture Coordinates

    glBindVertexArray(0);

    m_IndexCount = indices.size();
}

void OpenGLBufferManager::DeleteBuffers()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
}

void OpenGLBufferManager::Bind()
{
    glBindVertexArray(m_VAO);
}

void OpenGLBufferManager::Unbind()
{
    glBindVertexArray(0);
}

void OpenGLBufferManager::Draw()
{
    Bind();
    glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, nullptr);
    Unbind();
}
