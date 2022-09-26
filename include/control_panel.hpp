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
    void setSerialDevice(const char* dev);
    struct SerialLog
    {
        ControlPanel* Cp;
        ImGuiTextBuffer Buf;
        ImVector<int> LineOffsets;
        bool AutoScroll;
        bool Connected;

        SerialLog(ControlPanel* cp);
        void Clear();
        void AddLog(const char* fmt, ...) IM_FMTARGS(2);
        void Draw(const char* title, bool* p_open = NULL);
    } m_LogPanel;

};
