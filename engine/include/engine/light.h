#pragma once

#include <glm/glm.hpp>

struct PointLight {
    glm::vec3 position;
    glm::vec3 color;
    float intensity;
};

struct DirectionalLight {
    glm::vec3 position;
    glm::vec3 color;
    float intensity;

    glm::vec3 direction;
    float angle;
};