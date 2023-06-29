#include "scene_view.hpp"

#include "core/input.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>

#include "imgui.h"

SceneView::SceneView(int width, int height, CameraType cameraType)
    : m_Width(width), m_Height(height), m_IsSceneInteractive(false), m_MouseFirstEnter(true),
    m_CameraType(cameraType), m_Framebuffer(std::make_unique<renderer::GLFrameBuffer>())
{
    if (m_CameraType == CameraType::ARCBALL) {
        m_Camera = std::make_unique<ArcballCamera>(glm::vec3(0, 0, 6), glm::vec3(0), m_Width / m_Height);
    } else {
        m_Camera = std::make_unique<FpvCamera>(glm::vec3(0, 0, 6), glm::vec3(0, 0, -1), m_Width / m_Height);
    }

    m_Framebuffer->CreateBuffers(m_Width, m_Height);
    InitializeEntities();
}

SceneView::~SceneView()
{
    m_Framebuffer->DeleteBuffers();
}

void SceneView::InitializeEntities()
{
    std::vector<Vertex> cube_vertices;

    cube_vertices.push_back({glm::vec3(-1.0f, -1.0f, 1.0f)});  // front bottom-left  0
    cube_vertices.push_back({glm::vec3(-1.0f, 1.0f, 1.0f)});   // front top-left     1
    cube_vertices.push_back({glm::vec3(1.0f, 1.0f, 1.0f)});    // front top-right    2
    cube_vertices.push_back({glm::vec3(1.0f, -1.0f, 1.0f)});   // front bottom-right 3
    cube_vertices.push_back({glm::vec3(-1.0f, -1.0f, -1.0f)}); // back bottom-left   4
    cube_vertices.push_back({glm::vec3(-1.0f, 1.0f, -1.0f)});  // back top-left      5
    cube_vertices.push_back({glm::vec3(1.0f, 1.0f, -1.0f)});   // back top-right     6
    cube_vertices.push_back({glm::vec3(1.0f, -1.0f, -1.0f)});  // back bottom-right  7

    // Counter clock-wise winding
    std::vector<unsigned int> cube_indices{
        0, 2, 1, 0, 3, 2, // front face
        7, 5, 6, 7, 4, 5, // back face
        1, 6, 5, 1, 2, 6, // top face
        4, 3, 0, 4, 7, 3, // bottom face
        4, 1, 5, 4, 0, 1, // left face
        3, 6, 2, 3, 7, 6  // right face
    };

    std::shared_ptr<Entity> e1 = std::make_shared<Entity>();
    e1->Initialize(cube_vertices,
                   cube_indices,
                   GL_TRIANGLES,
                   "res/shaders/basic_vs.glsl",
                   "res/shaders/basic_fs.glsl");
    e1->SetScale(glm::scale(glm::mat4(1.0f), glm::vec3(0.5f)));

    std::vector<Vertex> plane_vertices;
    std::vector<unsigned int> plane_indices;

    int slices = 50;
    for (int j = 0; j <= slices; ++j)
    {
        for (int i = 0; i <= slices; ++i)
        {
            float x = (float)i / (float)slices;
            float y = 0;
            float z = (float)j / (float)slices;
            plane_vertices.push_back({glm::vec3(x, y, z)});
        }
    }

    for (int j = 0; j < slices; ++j)
    {
        for (int i = 0; i < slices; ++i)
        {
            int row1 = j * (slices + 1);
            int row2 = (j + 1) * (slices + 1);

            plane_indices.push_back(row1 + 1);
            plane_indices.push_back(row1 + i + 1);
            plane_indices.push_back(row1 + i + 1);
            plane_indices.push_back(row2 + i + 1);

            plane_indices.push_back(row2 + i + 1);
            plane_indices.push_back(row2 + i);
            plane_indices.push_back(row2 + i);
            plane_indices.push_back(row1 + i);
        }
    }

    std::shared_ptr<Entity> e2 = std::make_shared<Entity>();
    e2->Initialize(plane_vertices,
                   plane_indices,
                   GL_LINES,
                   "res/shaders/basic_vs.glsl",
                   "res/shaders/basic_fs.glsl");
    float scale = 100.0f;
    e2->SetScale(glm::scale(glm::mat4(1.0f), glm::vec3(scale)));
    e2->SetTranslation(glm::translate(glm::mat4(1.0f), glm::vec3(-scale / 2, 0.0f, -scale / 2)));

    m_Entities.push_back({"plane", e2});
    m_Entities.push_back({"cube", e1});
}

std::shared_ptr<Entity> SceneView::GetEntity(const std::string& name)
{
    for (auto entity : m_Entities)
    {
        if (entity.first == name)
        {
            return entity.second;
        }
    }

    std::cout << "No entity found with name: " << name << std::endl;
    return {};
}

void SceneView::SetSceneInteractive(bool interactive)
{
    m_IsSceneInteractive = interactive;
    m_MouseFirstEnter = true;
    if (interactive)
    {
        glfwSetInputMode(Input::s_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
    }
    else
    {
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
    }
}

void SceneView::SetCameraType(const CameraType& type)
{
    m_CameraType = type;
    if (m_CameraType == CameraType::ARCBALL) {
        m_Camera = std::make_unique<ArcballCamera>(glm::vec3(0, 0, 6), glm::vec3(0), m_Width / m_Height);
    } else {
        m_Camera = std::make_unique<FpvCamera>(glm::vec3(0, 0, 6), glm::vec3(0, 0, -1), m_Width / m_Height);
        m_MouseFirstEnter = true;
    }
}

void SceneView::PrintEntities()
{
    for (auto& e : m_Entities) {
        std::cout << e.first << " " << e.second->GetID() << std::endl;
    }
}

void SceneView::Update(float dt)
{
    if (m_IsSceneInteractive)
    {
        auto v = Input::GetMousePosition();
        if (m_MouseFirstEnter)
        {
            m_LastMousePosition = std::make_pair(v.first, v.second);
            m_MouseFirstEnter = false;
        }
        double xChange = std::get<0>(v) - m_LastMousePosition.first;
        double yChange = m_LastMousePosition.second - std::get<1>(v);

        m_LastMousePosition = std::make_pair(std::get<0>(v), std::get<1>(v));

        m_Camera->ProcessKeyPress(dt);
        m_Camera->ProcessMouseMovement(xChange, yChange, dt);
    }
}

void SceneView::Draw()
{
    m_Framebuffer->Bind();

    glClearColor(0.17f, 0.17f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (auto entity : m_Entities)
    {
        entity.second->Draw(m_Camera->GetViewProjectionMatrix());
    }

    m_Framebuffer->Unbind();

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoMove;

    ImGuiStyle& style = ImGui::GetStyle();
    style.ChildBorderSize = 0.0f;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Scene", NULL, window_flags);
    if (!m_IsSceneInteractive)
        SetSceneInteractive(ImGui::IsWindowHovered() && Input::IsKeyPressed(GLFW_KEY_G));
    ImGui::PopStyleVar();
    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    if (viewportPanelSize.x != m_Width || viewportPanelSize.y != m_Height)
    {
        m_Width = viewportPanelSize.x;
        m_Height = viewportPanelSize.y;
        m_Camera->SetAspectRatio(m_Width / m_Height);
    }
    unsigned int textureID = m_Framebuffer->GetTextureID();
    ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_Width, m_Height }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
    ImGui::End();
}
