#include "../include/window.hpp"

static void glfw_error_callback(int code, const char* description)
{
    fprintf(stderr, "GLFW ERROR: %d -> '%s'\n", code, description);
}

void GLWindow::glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto win = static_cast<GLWindow*>(glfwGetWindowUserPointer(window));

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        win->m_IsRunning = false;
}

void GLWindow::glfw_cursor_callback(GLFWwindow* window, double posX, double posY)
{
    auto win = reinterpret_cast<GLWindow*>(glfwGetWindowUserPointer(window));
    (void)win;
}

void GLWindow::glfw_scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
    auto win = reinterpret_cast<GLWindow*>(glfwGetWindowUserPointer(window));
    (void)win;
}

GLWindow::GLWindow(unsigned int width, unsigned int height, const std::string& title)
    : m_Window(nullptr), m_Width(width), m_Height(height), m_Title(title), m_IsRunning(true)
{
    // TODO: Initialize OpenGL and UI Context
}

GLWindow::~GLWindow()
{
    // TODO: De-initialize OpenGL and UI Context
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

void GLWindow::Initialize()
{
    glfwSetErrorCallback(glfw_error_callback);
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);

    glfwSetWindowUserPointer(m_Window, this);

    glfwSetKeyCallback(m_Window, glfw_key_callback);
    glfwSetCursorPosCallback(m_Window, glfw_cursor_callback);
    glfwSetScrollCallback(m_Window, glfw_scroll_callback);

    glfwMakeContextCurrent(m_Window);
    glfwSwapInterval(1);
}

void GLWindow::Loop()
{
    glfwPollEvents();
    glfwSwapBuffers(m_Window);
}
