#include "engine/attributes/first_person_camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "engine/global.h"
#include "engine/common.h"
#include "engine/event.h"
#include "engine/application.h"

#include <string>

glm::mat4 FirstPersonCamera::construct_view(glm::vec3 eye, float pitch, float yaw) {
    float cos_pitch = cos(pitch);
    float sin_pitch = sin(pitch);
    float cos_yaw = cos(yaw);
    float sin_yaw = sin(yaw);

    glm::vec3 x_axis(cos_yaw, 0, -sin_yaw);
    glm::vec3 y_axis(sin_yaw * sin_pitch, cos_pitch, cos_yaw * sin_pitch);
    glm::vec3 z_axis(sin_yaw * cos_pitch, -sin_pitch, cos_pitch * cos_yaw);

    // Create a 4x4 view matrix from the right, up, forward and eye position vectors
    return {
        {x_axis.x, y_axis.x, z_axis.x, 0},
        {x_axis.y, y_axis.y, z_axis.y, 0},
        {x_axis.z, y_axis.z, z_axis.z, 0},
        {-glm::dot(x_axis, eye), -glm::dot(y_axis, eye), -glm::dot(z_axis, eye), 1}
    };
}

void FirstPersonCamera::handle_window_resize(int w, int h) {
    camera_t *camera = Application::get_camera(0);
    float ratio = 1.0f * w / h;

    camera->projection = glm::perspective(camera->vfov, ratio, 0.1f, 100.0f);
}

void FirstPersonCamera::handle_cursor_position(double x, double y) {
    camera_t *camera = Application::get_camera(0);

    camera->view = this->construct_view(
        camera->position,
        (y - 0.5) * PI,
        (x - 0.5) * TAU
    );

    camera->position = {0.0, 2.0, 4.0};
    camera->vfov = 45.0f;
}

void FirstPersonCamera::receive_event(Event event) {
    switch (event.type) {
    case Initialize:
        this->handle_cursor_position(0.5, 0.5);
        break;
    case CursorPosition:
        this->handle_cursor_position(DOUBLE(0), DOUBLE(1));
        break;
    case WindowResize:
        this->handle_window_resize(INT(0), INT(1));
        break;
    default:
        break;
    }
}
