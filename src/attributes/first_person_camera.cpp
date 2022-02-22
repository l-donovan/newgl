#include "newgl/attributes/first_person_camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "newgl/global.h"
#include "newgl/common.h"
#include "newgl/event.h"
#include "newgl/window.h"

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

void FirstPersonCamera::handle_cursor_position(double x, double y) {
    glm::mat4 view = this->construct_view(
        {0.0, 2.0, 4.0},
        (y - 0.5) * PI,
        (x - 0.5) * TAU);

    Window::view = view;
}

void FirstPersonCamera::receive_event(Event event) {
    switch (event.type) {
    case CursorPosition:
        this->handle_cursor_position(DOUBLE(0), DOUBLE(1));
        break;
    default:
        break;
    }
}
