#include "newgl/attributes/test.h"

#include "newgl/global.h"
#include "newgl/common.h"
#include "newgl/event.h"

#include <cmath>

Test::Test(EntityLayer *entity) {
    this->entity = entity;

    this->subscribe(Tick1);
    this->subscribe(Tick10);
    this->subscribe(Tick100);
}

void Test::receive_event(Event event) {
    switch (event.type) {
    case Tick1:
        this->entity->set_position(
            cos(0.1f * LONG(event, 0)),
            sin(0.05f * LONG(event, 0)),
            -cos(0.025f * LONG(event, 0)));
        break;
    case Tick10:
        break;
    case Tick100:
        break;
    default:
        break;
    }
}
