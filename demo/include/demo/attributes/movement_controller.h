#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "engine/global.h"
#include "engine/attribute.h"
#include "engine/common.h"
#include "engine/event.h"

class MovementController : public Attribute {
    private:
        void handle_initialize();
        void handle_key(int key, int scancode, int action, int mods);
        void handle_tick(long frame_count);

        glm::vec2 acceleration;
        glm::vec2 velocity;
        glm::vec2 max_velocity;
        glm::vec3 position;

        camera_t *camera;
    public:
        MovementController() {
            this->subscribe(EventType::Initialize);
            this->subscribe(EventType::Key);
            this->subscribe(EventType::Tick1);
        };

        void receive_event(Event event);
};