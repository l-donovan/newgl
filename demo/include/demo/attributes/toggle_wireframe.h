#pragma once

#include "engine/global.h"
#include "engine/attribute.h"
#include "engine/layer.h"
#include "engine/common.h"
#include "engine/event.h"

class ToggleWireframe : public Attribute {
    private:
        Layer* layer;

        void wireframe_on();
        void wireframe_off();
        void handle_key_event(int key, int scancode, int action, int mods);

        bool wireframe_enabled = false;
    public:
        ToggleWireframe(Layer *layer) : layer(layer) {
            this->subscribe(EventType::BeginDraw);
            this->subscribe(EventType::EndDraw);
            this->subscribe(EventType::Key);
        };

        void receive_event(Event event);
};
