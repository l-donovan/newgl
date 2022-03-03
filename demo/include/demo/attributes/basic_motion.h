#pragma once

#include "engine/global.h"
#include "engine/attribute.h"
#include "engine/common.h"
#include "engine/event.h"
#include "engine/layers/entity_layer.h"

class BasicMotion : public Attribute {
    private:
        EntityLayer* entity;

        float phase;
    public:
        BasicMotion(EntityLayer *entity, float phase = 0.0f) {
            this->subscribe(EventType::Tick1);
            this->entity = entity;
            this->phase = phase;
        };

        void receive_event(Event event);
};
