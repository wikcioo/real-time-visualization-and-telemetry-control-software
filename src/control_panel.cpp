#include "control_panel.hpp"
#include <GL/glew.h>

struct ExampleAppLog
{
    ImGuiTextBuffer     Buf;
    ImGuiTextFilter     Filter;
    ImVector<int>       LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
    bool                AutoScroll;  // Keep scrolling if already at the bottom.

    ExampleAppLog()
    {
        AutoScroll = true;
        Clear();
    }

    void    Clear()
    {
        Buf.clear();
        LineOffsets.clear();
        LineOffsets.push_back(0);
    }

    void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
    {
        int old_size = Buf.size();
        va_list args;
        va_start(args, fmt);
        Buf.appendfv(fmt, args);
        va_end(args);
        for (int new_size = Buf.size(); old_size < new_size; old_size++)
            if (Buf[old_size] == '\n')
                LineOffsets.push_back(old_size + 1);
    }

    void    Draw(const char* title, bool* p_open = NULL)
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
        ImGui::SameLine();
        Filter.Draw("Filter", -100.0f);

        ImGui::Separator();
        ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        if (clear)
            Clear();
        if (copy)
            ImGui::LogToClipboard();

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        const char* buf = Buf.begin();
        const char* buf_end = Buf.end();
        if (Filter.IsActive())
        {
            // In this example we don't use the clipper when Filter is enabled.
            // This is because we don't have a random access on the result on our filter.
            // A real application processing logs with ten of thousands of entries may want to store the result of
            // search/filter.. especially if the filtering function is not trivial (e.g. reg-exp).
            for (int line_no = 0; line_no < LineOffsets.Size; line_no++)
            {
                const char* line_start = buf + LineOffsets[line_no];
                const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                if (Filter.PassFilter(line_start, line_end))
                    ImGui::TextUnformatted(line_start, line_end);
            }
        }
        else
        {
            // The simplest and easy way to display the entire buffer:
            //   ImGui::TextUnformatted(buf_begin, buf_end);
            // And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward
            // to skip non-visible lines. Here we instead demonstrate using the clipper to only process lines that are
            // within the visible area.
            // If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them
            // on your side is recommended. Using ImGuiListClipper requires
            // - A) random access into your data
            // - B) items all being the  same height,
            // both of which we can handle since we an array pointing to the beginning of each line of text.
            // When using the filter (in the block of code above) we don't have random access into the data to display
            // anymore, which is why we don't use the clipper. Storing or skimming through the search result would make
            // it possible (and would be recommended if you want to search through tens of thousands of entries).
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
        }
        ImGui::PopStyleVar();

        if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);

        ImGui::EndChild();
        ImGui::End();
    }
};

ControlPanel::ControlPanel()
{
    try {
        m_SerialPort = std::make_unique<serial::SerialPort>("/dev/ttyACM0");
    } catch (serial::SerialPortException& e) {
        std::cout << e.what();
        m_SerialPort = nullptr;
    }
}

ControlPanel::~ControlPanel()
{
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
    static ExampleAppLog log;
    static bool p_open;
    ImGui::Begin("Serial Log", &p_open);
    if (m_SerialPort && m_SerialPort->IsDataReady()) {
        uint8_t buffer[32];
        memset(buffer, 0, 32);
        m_SerialPort->ReadData((uint8_t*)buffer, 32);
        log.AddLog("%s", buffer);
    }
    ImGui::End();
    log.Draw("Serial Log", &p_open);
}
