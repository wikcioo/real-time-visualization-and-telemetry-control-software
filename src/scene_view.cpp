#include "scene_view.hpp"
#include "input.hpp"

#include <GL/glew.h>
#include "imgui.h"

SceneView::SceneView(int width, int height)
    : m_Width(width), m_Height(height), m_IsWindowHovered(false), m_Camera(nullptr), m_Framebuffer(nullptr)
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

    vertices.push_back({glm::vec3(-1.0f, -1.0f,  1.0f)});   // front bottom-left  0
    vertices.push_back({glm::vec3(-1.0f,  1.0f,  1.0f)});   // front top-left     1
    vertices.push_back({glm::vec3(1.0f,  1.0f,  1.0f)});    // front top-right    2
    vertices.push_back({glm::vec3(1.0f, -1.0f,  1.0f)});    // front bottom-right 3
    vertices.push_back({glm::vec3(-1.0f, -1.0f, -1.0f)});   // back bottom-left   4
    vertices.push_back({glm::vec3(-1.0f,  1.0f, -1.0f)});   // back top-left      5
    vertices.push_back({glm::vec3(1.0f,  1.0f, -1.0f)});    // back top-right     6
    vertices.push_back({glm::vec3(1.0f, -1.0f, -1.0f)});    // back bottom-right  7

    // Counter clock-wise winding
    std::vector<unsigned int> indices {
        0, 2, 1, 0, 3, 2, // front face
        7, 5, 6, 7, 4, 5, // back face
        1, 6, 5, 1, 2, 6, // top face
        4, 3, 0, 4, 7, 3, // bottom face
        4, 1, 5, 4, 0, 1, // left face
        3, 6, 2, 3, 7, 6  // right face
    };

    std::shared_ptr<Entity> e1 = std::make_shared<Entity>();
    e1->Initialize(vertices, indices, "res/shaders/basic_vs.glsl", "res/shaders/basic_fs.glsl");
    m_Entities.insert({"square", e1});
}

static bool s_MouseFirstEnter = true;
void SceneView::Update(float dt)
{
    if (m_IsWindowHovered)
    {
        auto v = Input::GetMousePosition();
        if (s_MouseFirstEnter)
        {
            m_LastMousePosition = std::make_pair(v.first, v.second);
            s_MouseFirstEnter = false;
        }
        double xChange = std::get<0>(v) - m_LastMousePosition.first;
        double yChange = m_LastMousePosition.second - std::get<1>(v);

        m_LastMousePosition = std::make_pair(std::get<0>(v), std::get<1>(v));

        m_Camera->ProcessMouseMovement(xChange, yChange);

        if (Input::IsKeyPressed(GLFW_KEY_W))
            m_Camera->ProcessKeyboard(MovementDirection::FORWARD, dt);
        if (Input::IsKeyPressed(GLFW_KEY_S))
            m_Camera->ProcessKeyboard(MovementDirection::BACKWARD, dt);
        if (Input::IsKeyPressed(GLFW_KEY_A))
            m_Camera->ProcessKeyboard(MovementDirection::LEFT, dt);
        if (Input::IsKeyPressed(GLFW_KEY_D))
            m_Camera->ProcessKeyboard(MovementDirection::RIGHT, dt);
    }
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
    m_IsWindowHovered = ImGui::IsWindowHovered();
    ImGui::PopStyleVar();
    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    m_Width = viewportPanelSize.x;
    m_Height = viewportPanelSize.y;

    m_Camera->SetAspectRatio(m_Width / m_Height);
    unsigned int textureID = m_Framebuffer->GetTextureID();
    ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_Width, m_Height }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
    ImGui::End();
}
