#include "demo/attributes/toggle_wireframe.h"

#include "engine/global.h"
#include "engine/common.h"
#include "engine/event.h"

#include <string>

void ToggleWireframe::wireframe_on() {
    if (this->wireframe_enabled)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void ToggleWireframe::wireframe_off() {
    if (this->wireframe_enabled)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void ToggleWireframe::handle_key_event(int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        this->wireframe_enabled = !this->wireframe_enabled;
    }
}

void ToggleWireframe::receive_event(Event event) {
    switch (event.type) {
    case EventType::BeginDraw:
        if (LAYER(0) == this->layer)
            this->wireframe_on();
        break;
    case EventType::EndDraw:
        if (LAYER(0) == this->layer)
            this->wireframe_off();
        break;
    case EventType::Key:
        this->handle_key_event(INT(0), INT(1), INT(2), INT(3));
        break;
    default:
        break;
    }
}
