#include "demo/attributes/toggle_cursor.h"

#include "engine/application.h"
#include "engine/global.h"
#include "engine/common.h"
#include "engine/event.h"

void ToggleCursor::handle_key_event(int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS) {
        this->cursor_enabled = !this->cursor_enabled;

        if (this->cursor_enabled) {
            // Disable the cursor
            glfwSetInputMode(Application::win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } else {
            // Enable the cursor
            glfwSetInputMode(Application::win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}

void ToggleCursor::receive_event(Event event) {
    switch (event.type) {
    case EventType::Key:
        this->handle_key_event(INT(0), INT(1), INT(2), INT(3));
        break;
    default:
        break;
    }
}
