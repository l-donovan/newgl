#pragma once

#include "engine/attribute.h"
#include "engine/event.h"

#include <imgui.h>

#include <memory>

class GrabInput : public Attribute, public std::enable_shared_from_this<GrabInput> {
    private:
        bool has_focus = false;
        bool in_window = false;

        void handle_cursor_position(double x, double y);
        void handle_mouse_click(int button, int action, int mods);

        ImGuiIO io;
    public:
        GrabInput() {
            this->subscribe(EventType::CursorPosition);
            this->subscribe(EventType::MouseButton);
        };

        void receive_event(Event event);

        std::shared_ptr<GrabInput> getptr() {
            return shared_from_this();
        }
};
