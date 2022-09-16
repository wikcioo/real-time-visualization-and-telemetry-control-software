#pragma once

#include <iostream>
#include <string>
#include <memory>

#include "opengl_context.hpp"
#include "ui_context.hpp"

#include <GLFW/glfw3.h>

class GLWindow
{
public:
    GLWindow(unsigned int width, unsigned int height, const std::string& title);
    ~GLWindow();

    void Initialize();
    void Loop();

    inline bool GetWindowShouldClose() const { return glfwWindowShouldClose(m_Window); }
    inline unsigned int GetWidth() const { return m_Width; }
    inline unsigned int GetHeight() const { return m_Height; }
    inline GLFWwindow* GetWindow() const { return m_Window; }
private:
    GLFWwindow* m_Window;
    unsigned int m_Width, m_Height;
    std::string m_Title;
    double m_DeltaTime, m_LastTime;
    std::unique_ptr<OpenGLContext> m_GLContext;
    std::unique_ptr<UIContext> m_UIContext;
private:
    void updateTime();

    static void glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void glfw_cursor_callback(GLFWwindow* window, double posX, double posY);
    static void glfw_scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
};
