#pragma once

#include <glm/glm.hpp>

#include <imgui.h>

#include "engine/layers/imgui_layer.h"

#include <string>

using std::string;

class MaterialEditor : public ImguiLayer {
private:
    ImGuiWindowFlags window_flags;
public:
    MaterialEditor();

    void setup();
    void update();
    void draw(glm::mat4 view, glm::mat4 projection, camera_t camera);
    void teardown();

    void receive_resource(ResourceType type, string name, void *data);

    float color[3] = {0.6, 0.5, 0.6};
    float light_color[3] = {1.0, 1.0, 1.0};
    float light_pos[3] = {-2.0, 1.0, 3.0};
    float ambient_color[3] = {0.8, 0.8, 0.7};

    float ambient_strength = 0.1;
    float specular_strength = 0.5;
};