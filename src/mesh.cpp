#include "mesh.hpp"

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
    DeleteBuffers();
}

void Mesh::Initialize(std::vector<Vertex> vertices, std::vector<unsigned int> indices, unsigned int mode)
{
    m_Vertices = vertices;
    m_Indices = indices;
    m_Mode = mode;

    m_BufferManager = std::make_unique<renderer::GLVertexBuffer>();
    CreateBuffers();
}

void Mesh::LoadFromFile(const char *filepath)
{
    // TODO: Implement the function
}

void Mesh::CreateBuffers()
{
    m_BufferManager->CreateBuffers(m_Vertices, m_Indices);
}

void Mesh::DeleteBuffers()
{
    m_BufferManager->DeleteBuffers();
}

void Mesh::Bind()
{
    m_BufferManager->Bind();
}

void Mesh::Unbind()
{
    m_BufferManager->Unbind();
}

void Mesh::Draw()
{
    m_BufferManager->Draw(m_Mode);
}
