#include "control_panel.hpp"
#include <GL/glew.h>

ControlPanel::SerialLog::SerialLog(ControlPanel* cp)
{
    Cp = cp;
    AutoScroll = true;
    Connected = false;
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
    //ImGui::SameLine();
    static char port_buffer[32] = "";
    ImGui::InputText("port", port_buffer, 32, ImGuiInputTextFlags_CharsNoBlank);
    ImGui::SameLine();
    bool connection = ImGui::Button(Connected ? "Disconnect" : "Connect");

    ImGui::Separator();
    ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    if (clear)
        Clear();
    if (copy)
        ImGui::LogToClipboard();
    if (connection && !Connected)
    {
        const char *dev_start = "/dev/tty";
        if (strlen(port_buffer) > strlen(dev_start) && strncmp(dev_start, port_buffer, strlen(dev_start)) == 0) {
            Cp->setSerialDevice(port_buffer);
        } else {
            AddLog("Not a teletype\n");
        }
    }
    else if (connection && Connected)
    {
        Connected = false;
        Cp->m_SerialPort = nullptr;
        AddLog("Disconnected from %s\n", port_buffer);
    }

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

ControlPanel::ControlPanel()
    : m_SerialPort(nullptr), m_LogPanel{this}
{
}

ControlPanel::~ControlPanel()
{
}

void ControlPanel::setSerialDevice(const char* dev)
{
    try {
        m_SerialPort = std::make_unique<serial::SerialPort>(dev);
        m_LogPanel.AddLog("Successfully connected to %s\n", dev);
        m_LogPanel.Connected = true;
    } catch (serial::SerialPortException& e) {
        m_LogPanel.AddLog("Failed to connect to %s\nError: %s\n", dev, e.what());
        m_LogPanel.Connected = false;
    }
}

void ControlPanel::Draw(std::shared_ptr<SceneView> scene)
{
    static bool demo = true;
    ImGui::ShowDemoWindow(&demo);
    static bool metrics = true;
    ImGui::ShowMetricsWindow(&metrics);
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

    // Serial Log
    static bool p_open;
    ImGui::Begin("Serial Log", &p_open);
    if (m_SerialPort && m_SerialPort->IsDataReady()) {
        uint8_t buffer[32];
        memset(buffer, 0, 32);
        m_SerialPort->ReadData((uint8_t*)buffer, 32);
        m_LogPanel.AddLog("%s", buffer);
    }
    ImGui::End();
    m_LogPanel.Draw("Serial Log", &p_open);
}
