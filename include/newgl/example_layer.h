#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>

#include "common.h"
#include "layer.h"

class ExampleLayer : public Layer {
    private:
        unsigned int VBO, VAO;
        GLuint vbo_vertices, vbo_colors, ibo_faces;
    public:
        void setup();
        void draw(glm::mat4 view, glm::mat4 projection, camera_t camera);
        void update();
        void teardown();
};
