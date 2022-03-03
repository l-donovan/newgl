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
        ImGui::Text("Metallic");
        ImGui::TableSetColumnIndex(1);
        ImGui::SliderFloat("##metallic", &this->metallic, 0.0f, 1.0f, "%.3f");

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Roughness");
        ImGui::TableSetColumnIndex(1);
        ImGui::SliderFloat("##roughness", &this->roughness, 0.0f, 1.0f, "%.3f");

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Reflectance");
        ImGui::TableSetColumnIndex(1);
        ImGui::SliderFloat("##reflectance", &this->reflectance, 0.0f, 1.0f, "%.3f");

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Clear coat");
        ImGui::TableSetColumnIndex(1);
        ImGui::SliderFloat("##clear_coat", &this->clear_coat, 0.0f, 1.0f, "%.3f");

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Clear coat roughness");
        ImGui::TableSetColumnIndex(1);
        ImGui::SliderFloat("##clear_coat_roughness", &this->clear_coat_roughness, 0.0f, 1.0f, "%.3f");

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Color");
        ImGui::TableSetColumnIndex(1);
        ImGui::ColorPicker3("##color", this->color, misc_flags | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);

        ImGui::EndTable();
    }

    ImGui::End();
}

void MaterialEditor::teardown() {

}

void MaterialEditor::receive_resource(ResourceType type, string name, void *data) {

}