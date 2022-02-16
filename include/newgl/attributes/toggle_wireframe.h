#pragma once

#include "../global.h"
#include "../attribute.h"
#include "../layer.h"
#include "../common.h"
#include "../event.h"

class ToggleWireframe : public Attribute {
    private:
        Layer* layer;

        void wireframe_on();
        void wireframe_off();
        void handle_key_event(int key, int scancode, int action, int mods);

        bool wireframe_enabled = false;
    public:
        ToggleWireframe(Layer *layer) : layer(layer) {
            this->subscribe(BeginDraw);
            this->subscribe(EndDraw);
            this->subscribe(Key);
        };

        void receive_event(Event event);
};
