#pragma once

#include "imgui.h"

class GLWindow;

class UIContext
{
public:
    UIContext(GLWindow* window);
    ~UIContext();

    void Initialize();
    void BeginFrame();
    void EndFrame();
private:
    GLWindow* m_Window;
};
