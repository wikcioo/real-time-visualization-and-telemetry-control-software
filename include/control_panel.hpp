#pragma once

#include "scene_view.hpp"
#include "imgui.h"

class ControlPanel
{
public:
    ControlPanel();
    ~ControlPanel();

    void Draw(std::shared_ptr<SceneView> scene);
private:
};
