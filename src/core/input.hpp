#pragma once

#include "window.hpp"

class Input
{
public:
    static void Initialize(GLFWwindow* window)
    {
        Input::s_Window = window;
    }

    static bool IsKeyPressed(int keycode)
    {
        auto state = glfwGetKey(s_Window, keycode);
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    static bool IsMouseButtonPressed(int button)
    {
        auto state = glfwGetMouseButton(s_Window, button);
        return state == GLFW_PRESS;
    }

    static std::pair<double, double> GetMousePosition()
    {
        double x, y;
        glfwGetCursorPos(s_Window, &x, &y);
        return {x, y};
    }

    static GLFWwindow* s_Window;
};
