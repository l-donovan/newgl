#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "engine/global.h"
#include "engine/attribute.h"
#include "engine/common.h"
#include "engine/event.h"

#include "demo/custom_event.h"

class FirstPersonCamera : public Attribute {
    private:
        void handle_initialize();
        void handle_cursor_position(double x, double y);
        void handle_window_resize(int w, int h);
        void handle_scroll(double x_offset, double y_offset);

        glm::mat4 construct_view(glm::vec3 eye, float pitch, float yaw);

        camera_t *camera;

        double last_x;
        double last_y;

        double horizontal_speed = 2.0;
        double vertical_speed = 2.0;
    public:
        FirstPersonCamera() {
            this->subscribe(EventType::Initialize);
            this->subscribe(EventType::CursorPosition);
            this->subscribe(EventType::WindowResize);
            this->subscribe(EventType::Scroll);
            this->subscribe((EventType) CustomEventType::RecalculateView);
        };

        void receive_event(Event event);
};
