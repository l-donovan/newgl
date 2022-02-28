#include "demo/attributes/basic_motion.h"

#include "engine/global.h"
#include "engine/common.h"
#include "engine/event.h"

#include <cmath>

void BasicMotion::receive_event(Event event) {
    switch (event.type) {
    case EventType::Tick1:
        this->entity->set_position(
            cos(0.1f * LONG(0) + this->phase),
            sin(0.05f * LONG(0) + this->phase),
            -cos(0.025f * LONG(0) + this->phase));
        break;
    default:
        break;
    }
}
