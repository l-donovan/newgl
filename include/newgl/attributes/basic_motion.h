#pragma once

#include "../global.h"
#include "../attribute.h"
#include "../entity_layer.h"
#include "../common.h"
#include "../event.h"

class BasicMotion : public Attribute {
    private:
        EntityLayer* entity;
    public:
        BasicMotion(EntityLayer *entity);

        void receive_event(Event event);
};
