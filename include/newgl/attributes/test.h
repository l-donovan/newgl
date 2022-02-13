#pragma once

#include "../global.h"
#include "../attribute.h"
#include "../entity_layer.h"
#include "../common.h"
#include "../event.h"

class Test : public Attribute {
    private:
        EntityLayer* entity;
    public:
        Test(EntityLayer *entity);

        void receive_event(Event event);
};
