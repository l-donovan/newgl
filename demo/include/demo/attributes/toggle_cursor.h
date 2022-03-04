#pragma once

#include "engine/global.h"
#include "engine/attribute.h"
#include "engine/layer.h"
#include "engine/common.h"
#include "engine/event.h"

class ToggleCursor : public Attribute, public std::enable_shared_from_this<ToggleCursor> {
    private:
        void handle_key_event(int key, int scancode, int action, int mods);

        bool cursor_enabled = false;
    public:
        ToggleCursor() {
            this->subscribe(EventType::Key);
        };

        void receive_event(Event event);

        std::shared_ptr<ToggleCursor> getptr() {
            return shared_from_this();
        }
};
