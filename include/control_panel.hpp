#pragma once

#include "scene_view.hpp"
#include "imgui.h"
#include "serial_port.hpp"

class ControlPanel
{
public:
    ControlPanel();
    ~ControlPanel();

    void Draw(std::shared_ptr<SceneView> scene);
private:
    std::unique_ptr<serial::SerialPort> m_SerialPort;
};
