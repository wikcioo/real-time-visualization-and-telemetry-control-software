#include "opengl_buffer_manager.hpp"

#include <GL/glew.h>

namespace renderer {

    GLVertexBuffer::GLVertexBuffer()
        : m_VAO(0), m_VBO(0), m_EBO(0), m_IndexCount(0)
    {
    }

    GLVertexBuffer::~GLVertexBuffer()
    {
        DeleteBuffers();
    }

    void GLVertexBuffer::CreateBuffers(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
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

    void GLVertexBuffer::DeleteBuffers()
    {
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
        glDeleteBuffers(1, &m_EBO);
    }

    void GLVertexBuffer::Bind()
    {
        glBindVertexArray(m_VAO);
    }

    void GLVertexBuffer::Unbind()
    {
        glBindVertexArray(0);
    }

    void GLVertexBuffer::Draw(unsigned int mode)
    {
        Bind();
        glDrawElements(mode, m_IndexCount, GL_UNSIGNED_INT, nullptr);
        Unbind();
    }


    GLFrameBuffer::GLFrameBuffer()
        : m_FBO(0), m_TextureID(0), m_Width(0), m_Height(0)
    {
    }

    GLFrameBuffer::~GLFrameBuffer()
    {
        DeleteBuffers();
    }

    void GLFrameBuffer::CreateBuffers(int width, int height)
    {
        m_Width = width;
        m_Height = height;

        glGenFramebuffers(1, &m_FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

        glGenTextures(1, &m_TextureID);
        glBindTexture(GL_TEXTURE_2D, m_TextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureID, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            fprintf(stderr, "Error framebuffer not complete\n");

        Unbind();
    }

    void GLFrameBuffer::DeleteBuffers()
    {
        glDeleteFramebuffers(1, &m_FBO);
        glDeleteTextures(1, &m_TextureID);
    }

    void GLFrameBuffer::Bind()
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, m_Width, m_Height);
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    }

    void GLFrameBuffer::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

}
