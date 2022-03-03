#pragma once

#include "engine/global.h"
#include "engine/attribute.h"
#include "engine/common.h"
#include "engine/event.h"
#include "engine/layers/text_layer.h"

class CaptureFramerate : public Attribute {
    private:
        TextLayer* text_layer;

        void handle_framerate(float fps, float percent);
    public:
        CaptureFramerate(TextLayer *text_layer) : text_layer(text_layer) {
            this->subscribe(EventType::Framerate);
        };

        void receive_event(Event event);
};
