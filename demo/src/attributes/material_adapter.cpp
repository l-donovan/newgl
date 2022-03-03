#include "demo/attributes/material_adapter.h"

#include "engine/common.h"

void MaterialAdapter::handle_tick10() {
    this->material->set("color", glm::make_vec3(this->editor->color));
    this->material->set("metallic", this->editor->metallic);
    this->material->set("roughness", this->editor->roughness);
    this->material->set("reflectance", this->editor->reflectance);
    this->material->set("clear_coat", this->editor->clear_coat);
    this->material->set("clear_coat_roughness", this->editor->clear_coat_roughness);
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