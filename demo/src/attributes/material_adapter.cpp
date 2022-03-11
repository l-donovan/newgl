#include "demo/attributes/material_adapter.h"

#include "engine/common.h"

void MaterialAdapter::handle_tick10() {
    this->material->set("color", glm::make_vec3(this->editor->color));
    this->material->set("light_pos", glm::make_vec3(this->editor->light_pos));
    this->material->set("light_color", glm::make_vec3(this->editor->light_color));
    this->material->set("ambient_strength", this->editor->ambient_strength);
    this->material->set("specular_strength", this->editor->specular_strength);
    this->material->set("ambient_color", glm::make_vec3(this->editor->ambient_color));
}

void MaterialAdapter::receive_event(Event event) {
    switch (event.type) {
    case EventType::Tick10:
        this->handle_tick10();
        break;
    default:
        break;
    }
}