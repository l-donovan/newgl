#include "demo/attributes/movement_controller.h"
#include "demo/custom_event.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <cmath>

#include "engine/application.h"

void MovementController::handle_initialize() {
    this->acceleration = {0.0f, 0.0f};
    this->velocity = {0.0f, 0.0f};
    this->max_velocity = {0.3f, 0.3f};
    this->position = {0.0, 2.0, 4.0};

    this->camera = Application::get_camera(0);
    this->camera->position = this->position;
}

void MovementController::handle_key(int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_W) {
        if (action == GLFW_PRESS) {
            this->acceleration.x = 0.1f;
        } else if (action == GLFW_RELEASE) {
            this->acceleration.x = 0.0f;
        }
    } else if (key == GLFW_KEY_S) {
        if (action == GLFW_PRESS) {
            this->acceleration.x = -0.1f;
        } else if (action == GLFW_RELEASE) {
            this->acceleration.x = 0.0f;
        }
    } else if (key == GLFW_KEY_A) {
        if (action == GLFW_PRESS) {
            this->acceleration.y = 0.1f;
        } else if (action == GLFW_RELEASE) {
            this->acceleration.y = 0.0f;
        }
    } else if (key == GLFW_KEY_D) {
        if (action == GLFW_PRESS) {
            this->acceleration.y = -0.1f;
        } else if (action == GLFW_RELEASE) {
            this->acceleration.y = 0.0f;
        }
    }
}

void MovementController::handle_tick(long frame_count) {
    this->velocity = {
        fmax(fmin(this->velocity.x + this->acceleration.x, this->max_velocity.x), -this->max_velocity.x),
        fmax(fmin(this->velocity.y + this->acceleration.y, this->max_velocity.y), -this->max_velocity.y),
    };

    // Friction
    this->velocity.x *= 0.9;
    this->velocity.y *= 0.9;

    this->position = {
        this->position.x + this->velocity.x * -sin(this->camera->yaw) + this->velocity.y * -sin(this->camera->yaw + PI / 2.0f),
        this->position.y + this->velocity.x * sin(this->camera->pitch),
        this->position.z + this->velocity.x * -cos(this->camera->yaw) + this->velocity.y * -cos(this->camera->yaw + PI / 2.0f)
    };

    this->camera->position = this->position;

    if (abs(this->velocity.x) > 0.002 || abs(this->velocity.y) > 0.002) {
        Application::send_event({(EventType) CustomEventType::RecalculateView, {}});
    }
}

void MovementController::receive_event(Event event) {
    switch (event.type) {
    case EventType::Initialize:
        this->handle_initialize();
        break;
    case EventType::Key:
        this->handle_key(INT(0), INT(1), INT(2), INT(3));
        break;
    case EventType::Tick1:
        this->handle_tick(LONG(0));
        break;
    default:
        break;
    }
}