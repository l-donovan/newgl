#include "demo/layers/material_editor.h"

#include <imgui.h>

MaterialEditor::MaterialEditor() {

}

void MaterialEditor::setup() {
    this->window_flags = 0;
}

void MaterialEditor::update() {

}

void MaterialEditor::draw(glm::mat4 view, glm::mat4 projection, camera_t camera) {
    auto p_open = new bool;
    *p_open = true;

    ImGuiColorEditFlags misc_flags = 0;

    if (!ImGui::Begin("Material Editor", p_open, this->window_flags)) {
        ImGui::End();
        return;
    }

    if (ImGui::BeginTable("material", 2)) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Color");
        ImGui::TableSetColumnIndex(1);
        ImGui::ColorPicker3("##color", this->color, misc_flags | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Light color");
        ImGui::TableSetColumnIndex(1);
        ImGui::ColorPicker3("##light_color", this->light_color, misc_flags | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Light position X");
        ImGui::TableSetColumnIndex(1);
        ImGui::SliderFloat("##pos_x", &this->light_pos[0], -10.0f, 10.0f, "%.3f");

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Light position Y");
        ImGui::TableSetColumnIndex(1);
        ImGui::SliderFloat("##pos_y", &this->light_pos[1], -10.0f, 10.0f, "%.3f");

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Light position Z");
        ImGui::TableSetColumnIndex(1);
        ImGui::SliderFloat("##pos_z", &this->light_pos[2], -10.0f, 10.0f, "%.3f");

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Ambient color");
        ImGui::TableSetColumnIndex(1);
        ImGui::ColorPicker3("##ambient_color", this->ambient_color, misc_flags | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Ambient strength");
        ImGui::TableSetColumnIndex(1);
        ImGui::SliderFloat("##ambient_strength", &this->ambient_strength, 0.0f, 1.0f, "%.3f");

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Specular strength");
        ImGui::TableSetColumnIndex(1);
        ImGui::SliderFloat("##specular_strength", &this->specular_strength, 0.0f, 1.0f, "%.3f");

        ImGui::EndTable();
    }

    ImGui::End();
}

void MaterialEditor::teardown() {

}

void MaterialEditor::receive_resource(ResourceType type, string name, void *data) {

}