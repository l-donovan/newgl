#include "demo/attributes/grab_input.h"
#include "demo/custom_event.h"

#include "engine/application.h"
#include "engine/global.h"
#include "engine/common.h"
#include "engine/event.h"

void GrabInput::handle_key_event(int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS) {
        this->cursor_enabled = !this->cursor_enabled;

        if (this->cursor_enabled) {
            // Enable the cursor
            glfwSetInputMode(Application::win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            Application::request_exclusive_input(CursorPositionInput, this->getptr());
            Application::request_exclusive_input(ScrollInput, this->getptr());
            Application::request_exclusive_input(KeyInput, this->getptr());
        } else {
            // Disable the cursor
            glfwSetInputMode(Application::win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            Application::release_exclusive_input(CursorPositionInput);
            Application::release_exclusive_input(ScrollInput);
            Application::release_exclusive_input(KeyInput);
        }
    }
}

void GrabInput::receive_event(Event event) {
    switch (static_cast<int>(event.type)) {
    case EventType::Initialize:
        this->handle_key_event(GLFW_KEY_LEFT_CONTROL, 0, GLFW_PRESS, 0);
        break;
    case EventType::Key:
        this->handle_key_event(INT(0), INT(1), INT(2), INT(3));
        break;
    default:
        break;
    }
}
