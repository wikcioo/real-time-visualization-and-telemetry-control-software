#include "window.hpp"

static void glfw_error_callback(int code, const char* description)
{
    fprintf(stderr, "GLFW ERROR: %d -> '%s'\n", code, description);
}

void GLWindow::glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    auto win = static_cast<GLWindow*>(glfwGetWindowUserPointer(window));
    win->m_GLContext->SetViewport(width, height);
}

void GLWindow::glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto win = static_cast<GLWindow*>(glfwGetWindowUserPointer(window));

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(win->GetWindow(), true);
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
    : m_Window(nullptr), m_Width(width), m_Height(height), m_Title(title)
{
    m_GLContext = std::make_unique<OpenGLContext>(this);
    m_UIContext = std::make_unique<UIContext>(this);
}

GLWindow::~GLWindow()
{
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

    glfwSetFramebufferSizeCallback(m_Window, glfw_framebuffer_size_callback);
    glfwSetKeyCallback(m_Window, glfw_key_callback);
    glfwSetCursorPosCallback(m_Window, glfw_cursor_callback);
    glfwSetScrollCallback(m_Window, glfw_scroll_callback);

    glfwMakeContextCurrent(m_Window);
    glfwSwapInterval(1);

    m_GLContext->Initialize();
    m_UIContext->Initialize();
}

void GLWindow::Loop()
{
    glfwPollEvents();

    m_GLContext->Clear();
    m_UIContext->BeginFrame();
    ImGui::Begin("Hello, world!");
    ImGui::End();
    m_UIContext->EndFrame();

    glfwSwapBuffers(m_Window);
}
