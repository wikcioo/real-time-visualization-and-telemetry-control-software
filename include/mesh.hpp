#pragma once

#include <vector>
#include <memory>

#include "opengl_buffer_manager.hpp"

class Mesh
{
public:
    Mesh();
    ~Mesh();

    void Initialize(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
    void LoadFromFile(const char *filepath);
    void CreateBuffers();
    void DeleteBuffers();
    void Bind();
    void Unbind();
    void Draw();
private:
    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    std::unique_ptr<renderer::GLVertexBuffer> m_BufferManager;
};
