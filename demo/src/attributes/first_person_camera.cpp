#include "demo/attributes/first_person_camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "engine/global.h"
#include "engine/common.h"
#include "engine/event.h"
#include "engine/application.h"

void FirstPersonCamera::handle_initialize() {
    this->camera = Application::get_camera(0);
    this->camera->vfov = 45.0f;

    this->handle_cursor_position(0.5, 0.5);
}

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
    double delta_x = x - this->last_x;
    double delta_y = y - this->last_y;

    this->last_x = x;
    this->last_y = y;

    this->camera->yaw -= this->horizontal_speed * delta_x;
    this->camera->pitch -= this->vertical_speed * delta_y;

    this->camera->pitch = fmin(fmax(this->camera->pitch, -0.5 * PI), 0.5 * PI);

    this->camera->view = this->construct_view(
        this->camera->position,
        this->camera->pitch,
        this->camera->yaw
    );
}

void FirstPersonCamera::handle_scroll(double x_offset, double y_offset) {
    this->camera->vfov += y_offset;
    PLOGD << "New vFOV: " << this->camera->vfov;
    this->handle_window_resize(Application::width, Application::height);
}

void FirstPersonCamera::receive_event(Event event) {
    switch (static_cast<int>(event.type)) {
    case EventType::Initialize:
        this->handle_initialize();
        break;
    case EventType::CursorPosition:
        this->handle_cursor_position(DOUBLE(0), DOUBLE(1));
        break;
    case EventType::WindowResize:
        this->handle_window_resize(INT(0), INT(1));
        break;
    case EventType::Scroll:
        this->handle_scroll(DOUBLE(0), DOUBLE(1));
        break;
    case CustomEventType::RecalculateView:
        this->handle_cursor_position(this->last_x, this->last_y);
        break;
    default:
        break;
    }
}
