#include "control_panel.hpp"
#include <GL/glew.h>

ControlPanel::ControlPanel()
{
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
}
