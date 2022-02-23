#pragma once

#include "../global.h"
#include "../attribute.h"
#include "../text_layer.h"
#include "../common.h"
#include "../event.h"

class CaptureFramerate : public Attribute {
    private:
        TextLayer* text_layer;

        void handle_framerate(float fps, float percent);
    public:
        CaptureFramerate(TextLayer *text_layer) : text_layer(text_layer) {
            this->subscribe(Framerate);
        };

        void receive_event(Event event);
};
