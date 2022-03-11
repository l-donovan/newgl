#include "demo/attributes/grab_input.h"

#include "engine/application.h"
#include "engine/common.h"

#include <imgui.h>

void GrabInput::handle_cursor_position(double x, double y) {
    ImGuiIO io = ImGui::GetIO();

    bool prev = this->in_window;
    this->in_window = io.WantCaptureMouse;

    if (this->in_window && !prev) {
        Application::request_exclusive_input(CursorPositionInput, this->getptr());
        Application::request_exclusive_input(ScrollInput, this->getptr());
    } else if (!this->in_window && prev) {
        Application::release_exclusive_input(CursorPositionInput);
        Application::release_exclusive_input(ScrollInput);
    }
}

void GrabInput::handle_mouse_click(int button, int action, int mods) {
    bool prev = this->has_focus;
    this->has_focus = this->in_window;

    if (this->has_focus && !prev) {
        Application::request_exclusive_input(KeyInput, this->getptr());
    } else if (!this->has_focus && prev) {
        Application::release_exclusive_input(KeyInput);
    }
}

void GrabInput::receive_event(Event event) {
    switch (event.type) {
    case EventType::CursorPosition:
        this->handle_cursor_position(DOUBLE(0), DOUBLE(1));
        break;
    case EventType::MouseButton:
        this->handle_mouse_click(INT(0), INT(1), INT(2));
        break;
    default:
        break;
    }
}