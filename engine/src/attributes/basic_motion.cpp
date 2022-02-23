#include "engine/attributes/basic_motion.h"

#include "engine/global.h"
#include "engine/common.h"
#include "engine/event.h"

#include <cmath>

BasicMotion::BasicMotion(EntityLayer *entity) {
    this->entity = entity;

    this->subscribe(Tick1);
    this->subscribe(Tick10);
    this->subscribe(Tick100);
}

void BasicMotion::receive_event(Event event) {
    switch (event.type) {
    case Tick1:
        this->entity->set_position(
            cos(0.1f * LONG(0)),
            sin(0.05f * LONG(0)),
            -cos(0.025f * LONG(0)));
        break;
    case Tick10:
        break;
    case Tick100:
        break;
    default:
        break;
    }
}
