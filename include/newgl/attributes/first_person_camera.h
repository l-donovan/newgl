#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../global.h"
#include "../attribute.h"
#include "../common.h"
#include "../event.h"

class FirstPersonCamera : public Attribute {
    private:
        void handle_cursor_position(double x, double y);
        glm::mat4 construct_view(glm::vec3 eye, float pitch, float yaw);
    public:
        FirstPersonCamera() {
            this->subscribe(CursorPosition);
        };

        void receive_event(Event event);
};
