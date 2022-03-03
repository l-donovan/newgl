#include "engine/layers/imgui_layer.h"
#include "engine/application.h"

#include <imgui.h>

ImguiLayer::ImguiLayer() {

}

void ImguiLayer::setup() {
     
}

void ImguiLayer::update() {

}

void ImguiLayer::draw(glm::mat4 view, glm::mat4 projection, camera_t camera) {
    auto thing = new bool;
    *thing = true;

    ImGui::ShowDemoWindow(thing);
}

void ImguiLayer::teardown() {

}

void ImguiLayer::receive_resource(ResourceType type, string name, void *data) {

}