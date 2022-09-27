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

ControlPanel::ControlPanel()
    : m_SerialPort(nullptr), m_IsSerialDeviceConnected(false), m_LogPanel{this}, m_ConsolePanel{this}
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
    static bool stack_tool = true;
    ImGui::ShowStackToolWindow(&stack_tool);

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
    if (m_SerialPort && m_SerialPort->IsDataReady()) {
        uint8_t buffer[32];
        memset(buffer, 0, 32);
        m_SerialPort->ReadData((uint8_t*)buffer, 32);
        m_LogPanel.AddLog("%s", buffer);
    }
    m_LogPanel.Draw("Serial Log", &serial_log_open);

    // Serial Console
    static bool serial_console_open;
    m_ConsolePanel.Draw("Serial Console", &serial_console_open);
}
