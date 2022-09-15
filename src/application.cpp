#include "application.hpp"

Application::Application(const std::string& title)
{
    m_Window = std::make_unique<GLWindow>(1280, 720, title);
    m_Window->Initialize();
}

void Application::Run()
{
    while (!m_Window->GetWindowShouldClose())
    {
        m_Window->Loop();
    }
}
