#include <iostream>

#include "../include/opengl_context.hpp"
#include "../include/window.hpp"

OpenGLContext::OpenGLContext(GLWindow* window)
    : m_Window(window)
{
}

OpenGLContext::~OpenGLContext()
{
}

void OpenGLContext::Initialize()
{
    glewExperimental = true;
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "GLEW Error: Failed to initialize\n");
        return;
    }

    glEnable(GL_DEPTH_TEST);
}

void OpenGLContext::SetViewport(int width, int height)
{
    glViewport(0, 0, width, height);
}

void OpenGLContext::Clear()
{
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
