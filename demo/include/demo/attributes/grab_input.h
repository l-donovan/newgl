#pragma once

#include "engine/attribute.h"
#include "engine/event.h"

#include "demo/custom_event.h"

#include <memory>

class GrabInput : public Attribute, public std::enable_shared_from_this<GrabInput> {
    private:
        bool cursor_enabled = false;

        void handle_key_event(int key, int scancode, int action, int mods);
    public:
        GrabInput() {
            this->subscribe(EventType::Initialize);
            this->subscribe(EventType::Key);
        };

        void receive_event(Event event);

        std::shared_ptr<GrabInput> getptr() {
            return shared_from_this();
        }
};
