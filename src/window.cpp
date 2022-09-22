#include "input.hpp"
#include "window.hpp"

static void glfw_error_callback(int code, const char* description)
{
    fprintf(stderr, "GLFW ERROR: %d -> '%s'\n", code, description);
}

void GLWindow::updateTime()
{
    double now = glfwGetTime();
    m_DeltaTime = now - m_LastTime;
    m_LastTime = now;
}

GLWindow::GLWindow(unsigned int width, unsigned int height, const std::string& title)
    : m_Window(nullptr), m_Width(width), m_Height(height), m_Title(title), m_DeltaTime(0.0f), m_LastTime(0.0f)
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

    glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
    {
        auto win = static_cast<GLWindow*>(glfwGetWindowUserPointer(window));
        win->m_GLContext->SetViewport(width, height);
    });

    glfwMakeContextCurrent(m_Window);
    glfwSwapInterval(1);

    Input::Initialize(m_Window);
    m_GLContext->Initialize();
    m_UIContext->Initialize();
    m_SceneView = std::make_unique<SceneView>(m_Width, m_Height);
}

void GLWindow::Loop()
{
    glfwPollEvents();
    updateTime();

    m_GLContext->Clear();
    m_UIContext->BeginFrame();

    static bool demo = true;
    ImGui::ShowDemoWindow(&demo);

    m_SceneView->Update(m_DeltaTime);
    m_SceneView->Draw();

    m_UIContext->EndFrame();

    if (Input::IsKeyPressed(GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(m_Window, true);
    glfwSwapBuffers(m_Window);
}
