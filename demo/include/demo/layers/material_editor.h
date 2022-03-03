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
        float metallic = 1.0f;
        float roughness = 1.0f;
        float reflectance = 0.5f;
        float clear_coat = 0.1f;
        float clear_coat_roughness = 0.1f;
};