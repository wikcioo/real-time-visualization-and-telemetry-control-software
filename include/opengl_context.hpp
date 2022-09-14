#pragma once

#include <memory>

#include <GL/glew.h>

class GLWindow;

class OpenGLContext
{
public:
    OpenGLContext(GLWindow* window);
    ~OpenGLContext();

    void Initialize();
    void SetViewport(int width, int height);
    void Clear();
private:
    GLWindow* m_Window;
};
