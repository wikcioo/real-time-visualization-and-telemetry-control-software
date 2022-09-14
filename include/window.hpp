#pragma once

#include <iostream>
#include <string>
#include <memory>

#include <GLFW/glfw3.h>

class GLWindow
{
public:
    GLWindow(unsigned int width, unsigned int height, const std::string& title);
    ~GLWindow();

    void Initialize();
    void Loop();

    inline bool IsRunning() { return m_IsRunning; }
private:
    GLFWwindow* m_Window;
    unsigned int m_Width, m_Height;
    std::string m_Title;
    bool m_IsRunning;

    static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void glfw_cursor_callback(GLFWwindow* window, double posX, double posY);
    static void glfw_scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
};
