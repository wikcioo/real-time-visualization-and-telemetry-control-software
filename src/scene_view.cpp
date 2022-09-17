#include "scene_view.hpp"
#include <GL/glew.h>
#include "imgui.h"

SceneView::SceneView(int width, int height)
    : m_Width(width), m_Height(height), m_Camera(nullptr), m_Framebuffer(nullptr)
{
    m_Camera = std::make_unique<PerspectiveCamera>(glm::vec3(0, 0, 2), glm::vec3(0, 0, -1), m_Width / m_Height);
    m_Framebuffer = std::make_unique<renderer::GLFrameBuffer>();
    m_Framebuffer->CreateBuffers(m_Width, m_Height);
}

SceneView::~SceneView()
{
    m_Framebuffer->DeleteBuffers();
}

void SceneView::ProcessMouseMovement(double xChange, double yChange, int button)
{
    m_Camera->ProcessMouseMovement(xChange, yChange);
}

void SceneView::SetSize(int width, int height)
{
    m_Width = width;
    m_Height = height;
    m_Framebuffer->DeleteBuffers();
    m_Framebuffer->CreateBuffers(m_Width, m_Height);
}

void SceneView::Draw()
{
    m_Framebuffer->Bind();

    glClearColor(0.4f, 0.2f, 0.7f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_Framebuffer->Unbind();

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoMove;

    ImGuiStyle& style = ImGui::GetStyle();
    style.ChildBorderSize = 0.0f;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Scene", NULL, window_flags);
    ImGui::PopStyleVar();
    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    m_Width = viewportPanelSize.x;
    m_Height = viewportPanelSize.y;

    m_Camera->SetAspectRatio(m_Width / m_Height);
    unsigned int textureID = m_Framebuffer->GetTextureID();
    ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_Width, m_Height }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
    ImGui::End();
}
