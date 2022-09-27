#include "control_panel.hpp"
#include <GL/glew.h>
#include <sstream>

ControlPanel::SerialLog::SerialLog(ControlPanel* cp)
{
    Cp = cp;
    AutoScroll = true;
    Clear();
};

void ControlPanel::SerialLog::Clear()
{
    Buf.clear();
    LineOffsets.clear();
    LineOffsets.push_back(0);
};

void ControlPanel::SerialLog::AddLog(const char* fmt, ...)
{
    int old_size = Buf.size();
    va_list args;
    va_start(args, fmt);
    Buf.appendfv(fmt, args);
    va_end(args);
    for (int new_size = Buf.size(); old_size < new_size; old_size++)
        if (Buf[old_size] == '\n')
            LineOffsets.push_back(old_size + 1);
};

void ControlPanel::SerialLog::Draw(const char* title, bool* p_open)
{
    if (!ImGui::Begin(title, p_open))
    {
        ImGui::End();
        return;
    }

    // Options menu
    if (ImGui::BeginPopup("Options"))
    {
        ImGui::Checkbox("Auto-scroll", &AutoScroll);
        ImGui::EndPopup();
    }

    // Main window
    if (ImGui::Button("Options"))
        ImGui::OpenPopup("Options");
    ImGui::SameLine();
    bool clear = ImGui::Button("Clear");
    ImGui::SameLine();
    bool copy = ImGui::Button("Copy");

    ImGui::Separator();
    ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    if (clear)
        Clear();
    if (copy)
        ImGui::LogToClipboard();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    const char* buf = Buf.begin();
    const char* buf_end = Buf.end();
    ImGuiListClipper clipper;
    clipper.Begin(LineOffsets.Size);
    while (clipper.Step())
    {
        for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
        {
            const char* line_start = buf + LineOffsets[line_no];
            const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
            ImGui::TextUnformatted(line_start, line_end);
        }
    }
    clipper.End();
    ImGui::PopStyleVar();

    if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);

    ImGui::EndChild();
    ImGui::End();
}

ControlPanel::SerialConsole::SerialConsole(ControlPanel* cp)
{
    Cp = cp;
    AutoScroll = true;
    ScrollToBottom = false;

    ClearLog();
    memset(InputBuf, 0, sizeof(InputBuf));

    Commands.push_back("help");
    Commands.push_back("clear");
    Commands.push_back("connect <device>");
    Commands.push_back("disconnect");
}

ControlPanel::SerialConsole::~SerialConsole()
{
    ClearLog();
}

void ControlPanel::SerialConsole::ClearLog()
{
    for (int i = 0; i < Items.Size; i++)
        free(Items[i]);
    Items.clear();
}

void ControlPanel::SerialConsole::AddLog(const char* fmt, ...)
{
    // FIXME-OPT
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
    buf[IM_ARRAYSIZE(buf)-1] = 0;
    va_end(args);
    Items.push_back(Strdup(buf));
}

void ControlPanel::SerialConsole::Draw(const char* title, bool* p_open)
{
    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin(title, p_open))
    {
        ImGui::End();
        return;
    }

    std::string status = Cp->m_IsSerialDeviceConnected ? "connected to " + Cp->getSerialDeviceName() : "disconnected";
    ImGui::Text("Status: %s", status.c_str());
    ImGui::Separator();

    // Reserve enough left-over height for 1 separator + 1 input text
    const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
    for (int i = 0; i < Items.Size; i++)
    {
        const char* item = Items[i];
        ImVec4 color;
        bool has_color = false;
        if (strstr(item, "[error]"))          { color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true; }
        else if (strncmp(item, "# ", 2) == 0) { color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); has_color = true; }
        if (has_color)
            ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::TextUnformatted(item);
        if (has_color)
            ImGui::PopStyleColor();
    }

    if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
        ImGui::SetScrollHereY(1.0f);
    ScrollToBottom = false;

    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::Separator();

    // Command-line
    bool reclaim_focus = false;
    ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue;
    if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), input_text_flags))
    {
        char* s = InputBuf;
        Strtrim(s);
        if (s[0])
            ExecCommand(s);
        strcpy(s, "");
        reclaim_focus = true;
    }

    // Auto-focus on window apparition
    ImGui::SetItemDefaultFocus();
    if (reclaim_focus)
        ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

    ImGui::End();
}

void ControlPanel::SerialConsole::ExecCommand(const std::string& command_line)
{
    AddLog("# %s\n", command_line.c_str());

    // Split command line into tokens
    std::vector<std::string> tokens;
    std::istringstream ss(command_line);
    std::string token;
    while (ss >> token) tokens.push_back(token);

    // Process command
    if (Stricmp(command_line.c_str(), "clear") == 0)
    {
        ClearLog();
    }
    else if (Stricmp(command_line.c_str(), "help") == 0)
    {
        AddLog("Commands:");
        for (int i = 0; i < Commands.Size; i++)
            AddLog("- %s", Commands[i]);
    }
    else if (tokens.at(0) == "connect")
    {
        if (tokens.size() < 2)
        {
            AddLog("[error] wrong usage of the 'connect' command\n");
        }
        else
        {
            const char *device_prefix = "/dev/tty";
            if (strncmp(tokens.at(1).c_str(), device_prefix, strlen(device_prefix)) == 0)
            {
                Cp->setSerialDevice(tokens.at(1));
            }
            else
            {
                AddLog("Not a teletype\n");
            }
        }
    }
    else if (tokens.at(0) == "disconnect")
    {
        AddLog("Disconnected from %s\n", Cp->getSerialDeviceName().c_str());
        Cp->clearSerialDevice();
    }
    else
    {
        AddLog("Unknown command: '%s'\n", command_line.c_str());
    }

    // On command input, we scroll to bottom even if AutoScroll==false
    ScrollToBottom = true;
}

ControlPanel::OrientationGraphPlotter::OrientationGraphPlotter(int _samples, float error_threshold)
    : activated(false), samples(_samples), values_offset(0), err_threshold(error_threshold), prev_x(0.0f), prev_y(0.0f), prev_z(0.0f)
{
    Resize(samples);
}

ControlPanel::OrientationGraphPlotter::~OrientationGraphPlotter()
{
    values_offset = 0;
    prev_x = prev_y = prev_z = 0.0f;
    x_values.clear();
    y_values.clear();
    z_values.clear();
}

void ControlPanel::OrientationGraphPlotter::Resize(int _samples)
{
    x_values.resize(_samples);
    y_values.resize(_samples);
    z_values.resize(_samples);
}

// TODO: Fix the bug where the axis locks itself to the specific value for extended period of time
void ControlPanel::OrientationGraphPlotter::Filter(float& x, float& y, float& z)
{
    if (activated) {
        prev_x = x;
        prev_y = y;
        prev_z = z;
        activated = false;
        return;
    }

    if (std::abs(x - prev_x) > err_threshold) {
        x = prev_x;
    }
    if (std::abs(y - prev_y) > err_threshold) {
        y = prev_y;
    }
    if (std::abs(z - prev_z) > err_threshold) {
        z = prev_z;
    }

    prev_x = x;
    prev_y = y;
    prev_z = z;
}

void ControlPanel::OrientationGraphPlotter::AddValues(float x, float y, float z)
{
    x_values[values_offset] = x;
    y_values[values_offset] = y;
    z_values[values_offset] = z;
    values_offset = (values_offset + 1) % samples;
}

void ControlPanel::OrientationGraphPlotter::Plot(const char* title, bool* p_open)
{
    ImGui::Begin(title, p_open);
    ImVec2 size = ImGui::GetContentRegionAvail();
    static int plot_v_offset = 30;

    ImGui::Text("x-axis g-force");
    ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    ImGui::PlotLines("x-axis", x_values.data(), samples, values_offset, NULL, -1.0f, 1.0f, ImVec2(size.x, size.y / 3 - plot_v_offset));
    ImGui::PopStyleColor();
    ImGui::Dummy(ImVec2(0, 10));

    ImGui::Text("y-axis g-force");
    ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
    ImGui::PlotLines("y-axis", y_values.data(), samples, values_offset, NULL, -1.0f, 1.0f, ImVec2(size.x, size.y / 3 - plot_v_offset));
    ImGui::PopStyleColor();
    ImGui::Dummy(ImVec2(0, 10));

    ImGui::Text("z-axis g-force");
    ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.01f, 0.57f, 1.0f, 1.0f));
    ImGui::PlotLines("z-axis", z_values.data(), samples, values_offset, NULL, -1.0f, 1.0f, ImVec2(size.x, size.y / 3 - plot_v_offset));
    ImGui::PopStyleColor();

    ImGui::End();
}

ControlPanel::ControlPanel()
    : m_SerialPort(nullptr), m_IsSerialDeviceConnected(false), m_LogPanel{this}, m_ConsolePanel{this}, m_Plotter{360, 0.05f}
{
}

ControlPanel::~ControlPanel()
{
}

void ControlPanel::setSerialDevice(const std::string& dev)
{
    try {
        m_SerialPort = std::make_unique<serial::SerialPort>(dev);
        m_ConsolePanel.AddLog("Successfully connected to %s\n", dev.c_str());
        m_IsSerialDeviceConnected = true;
        m_Plotter.activated = true;
    } catch (serial::SerialPortException& e) {
        m_ConsolePanel.AddLog("Failed to connect to %s\nError: %s\n", dev.c_str(), e.what());
        m_IsSerialDeviceConnected = false;
    }
}

void ControlPanel::clearSerialDevice()
{
    m_SerialPort = nullptr;
    m_IsSerialDeviceConnected = false;
}

std::string ControlPanel::getSerialDeviceName()
{
    if (m_SerialPort)
        return m_SerialPort->GetDeviceName();
    return "unknown";
}

void ControlPanel::Draw(std::shared_ptr<SceneView> scene)
{
    static bool demo = true;
    ImGui::ShowDemoWindow(&demo);

    static bool wireframe_window = true;
    static bool wireframe_on = false;
    ImGui::Begin("Mesh Settings", &wireframe_window);
    ImGui::Checkbox("Wireframe On", &wireframe_on);
    if (wireframe_on) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    static ImVec4 cube_color = ImVec4(48/255.0f, 170/255.0f, 201/255.0f, 1.0f);
    ImGui::ColorEdit3("Cube color", (float*)&cube_color);
    auto entities = scene->GetEntities();
    int c = 1;
    if (ImGui::TreeNode("Entities"))
    {
        for (auto it = entities.begin(); it != entities.end(); it++)
        {
            ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
            ImGui::TreeNodeEx((void*)(intptr_t)c, node_flags, "id = %u -> %s", it->second->GetID(), it->first.c_str());
            c++;
        }
        ImGui::TreePop();
    }
    static int cam_nr = (int)scene->GetCameraType();
    ImGui::RadioButton("Arcball Camera", &cam_nr, 0); ImGui::SameLine();
    ImGui::RadioButton("Fpv Camera", &cam_nr, 1);
    ImGui::End();

    if (static_cast<int>(scene->GetCameraType()) != cam_nr) {
        scene->SetCameraType(static_cast<CameraType>(cam_nr));
    }

    auto cube = scene->GetEntity("cube");
    auto plane = scene->GetEntity("plane");

    cube->SetColor({cube_color.x, cube_color.y, cube_color.z});
    plane->SetColor({cube_color.x, cube_color.y, cube_color.z});

    // Raw Serial Log
    static bool serial_log_open;
    static float x_accel = 0.0f, y_accel = 0.0f, z_accel = 0.0f;

    if (m_SerialPort && m_SerialPort->IsDataReady()) {
        uint8_t buffer[32];
        memset(buffer, 0, 32);
        m_SerialPort->ReadData((uint8_t*)buffer, 32);

        std::string data = (char*)buffer;
        std::stringstream ss(data);
        ss >> x_accel >> y_accel >> z_accel;
        m_Plotter.Filter(x_accel, y_accel, z_accel);

        float roll = atan2(y_accel, z_accel) * 180/M_PI;
        float pitch = atan2(-x_accel, sqrt(y_accel*y_accel + z_accel*z_accel)) * 180/M_PI;
        glm::mat4 rot(1.0f);
        rot = glm::rotate(rot, glm::radians((float)pitch), glm::vec3(1, 0, 0));
        rot = glm::rotate(rot, glm::radians((float)roll), glm::vec3(0, 0, 1));
        scene->GetEntity("cube")->SetRotation(rot);

        m_LogPanel.AddLog("%s", buffer);

        // Update plotting values
        m_Plotter.AddValues(x_accel, y_accel, z_accel);
    }
    m_LogPanel.Draw("Serial Log", &serial_log_open);

    // Serial Console
    static bool serial_console_open;
    m_ConsolePanel.Draw("Serial Console", &serial_console_open);

    // Plotting
    static bool graphs_open = true;
    m_Plotter.Plot("Orientation Graphs", &graphs_open);
}
