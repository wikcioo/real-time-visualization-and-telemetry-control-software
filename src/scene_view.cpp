#include "scene_view.hpp"
#include <GL/glew.h>
#include "imgui.h"

SceneView::SceneView(int width, int height)
    : m_Width(width), m_Height(height), m_Camera(nullptr), m_Framebuffer(nullptr)
{
    m_Camera = std::make_unique<PerspectiveCamera>(glm::vec3(0, 0, 2), glm::vec3(0, 0, -1), m_Width / m_Height);
    m_Framebuffer = std::make_unique<renderer::GLFrameBuffer>();
    m_Framebuffer->CreateBuffers(m_Width, m_Height);

    InitializeEntities();
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

void SceneView::InitializeEntities()
{
    std::vector<Vertex> vertices;

    vertices.push_back({glm::vec3(-0.5f, -0.5f, 0.0f)});
    vertices.push_back({glm::vec3( 0.5f, -0.5f, 0.0f)});
    vertices.push_back({glm::vec3( 0.5f,  0.5f, 0.0f)});
    vertices.push_back({glm::vec3(-0.5f,  0.5f, 0.0f)});

    std::vector<unsigned int> indices{0, 1, 2, 2, 3, 0};

    std::shared_ptr<Entity> e1 = std::make_shared<Entity>();
    e1->Initialize(vertices, indices, "res/shaders/basic_vs.glsl", "res/shaders/basic_fs.glsl");
    m_Entities.insert({"square", e1});
}

void SceneView::Draw()
{
    m_Framebuffer->Bind();

    glClearColor(0.4f, 0.2f, 0.7f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (auto entity : m_Entities)
    {
        entity.second->Draw(m_Camera->GetViewProjection());
    }

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
