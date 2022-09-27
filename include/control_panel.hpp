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
    bool m_IsSerialDeviceConnected;

    struct SerialLog
    {
        ControlPanel* Cp;
        ImGuiTextBuffer Buf;
        ImVector<int> LineOffsets;
        bool AutoScroll;

        SerialLog(ControlPanel* cp);
        void Clear();
        void AddLog(const char* fmt, ...) IM_FMTARGS(2);
        void Draw(const char* title, bool* p_open = NULL);
    } m_LogPanel;

    struct SerialConsole
    {
        ControlPanel* Cp;
        char InputBuf[256];
        ImVector<char*> Items;
        ImVector<const char*> Commands;
        bool AutoScroll;
        bool ScrollToBottom;

        SerialConsole(ControlPanel* cp);
        ~SerialConsole();

        // Portable helpers
        static int   Stribeg(const char* s1, const char* s2)         { return strncmp(s1, s2, strlen(s2)) == 0; }
        static int   Stricmp(const char* s1, const char* s2)         { int d; while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; } return d; }
        static int   Strnicmp(const char* s1, const char* s2, int n) { int d = 0; while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; n--; } return d; }
        static char* Strdup(const char* s)                           { IM_ASSERT(s); size_t len = strlen(s) + 1; void* buf = malloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)s, len); }
        static void  Strtrim(char* s)                                { char* str_end = s + strlen(s); while (str_end > s && str_end[-1] == ' ') str_end--; *str_end = 0; }

        void ClearLog();
        void AddLog(const char* fmt, ...) IM_FMTARGS(2);
        void Draw(const char* title, bool* p_open);
        void ExecCommand(const std::string& command_line);
    } m_ConsolePanel;

    struct OrientationGraphPlotter
    {
        bool activated;
        int samples;
        int values_offset;
        float err_threshold;
        float prev_x, prev_y, prev_z;
        std::vector<float> x_values;
        std::vector<float> y_values;
        std::vector<float> z_values;

        OrientationGraphPlotter(int _samples, float error_threshold);
        ~OrientationGraphPlotter();

        void Resize(int _samples);
        void Filter(float& x, float& y, float& z);
        void AddValues(float x, float y, float z);
        void Plot(const char* title, bool* p_open);
    } m_Plotter;
private:
    void setSerialDevice(const std::string& dev);
    void clearSerialDevice();
    std::string getSerialDeviceName();
};
