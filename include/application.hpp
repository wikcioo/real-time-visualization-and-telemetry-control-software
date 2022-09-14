#pragma once

#include <string>
#include <memory>
#include "window.hpp"

class Application
{
public:
    Application(const std::string& title);
    void Run();
private:
    std::unique_ptr<GLWindow> m_Window;
};
