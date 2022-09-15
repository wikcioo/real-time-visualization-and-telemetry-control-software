#pragma once

#include <vector>
#include <iostream>
#include <glm/glm.hpp>

struct Vertex
{
    glm::vec3 Position;
};

namespace renderer {

    class GLVertexBuffer
    {
    public:
        GLVertexBuffer();
        ~GLVertexBuffer();

        void CreateBuffers(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
        void DeleteBuffers();

        void Bind();
        void Unbind();

        void Draw();
    private:
        unsigned int m_VAO, m_VBO, m_EBO, m_IndexCount;
    };

    class GLFrameBuffer
    {
    public:
        GLFrameBuffer();
        ~GLFrameBuffer();

        void CreateBuffers(int width, int height);
        void DeleteBuffers();

        void Bind();
        void Unbind();

        inline unsigned int GetTextureID() { return m_TextureID; }
    private:
        unsigned int m_FBO, m_TextureID;
        int m_Width, m_Height;
    };

}
