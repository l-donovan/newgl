#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>

#include "engine/common.h"
#include "engine/global.h"
#include "engine/layer.h"

#include <string>

using std::string;

class SkyLayer : public Layer {
    private:
        // VBOs and IBOs
        GLuint vbo_vertices = 0;
        GLuint ibo_faces = 0;

        // Textures
        GLuint texture_0 = 0;

        // Locations
        GLuint view_location = 0;
        GLuint projection_location = 0;
        GLuint texture_location = 0;
        GLint vertex_location = 0;
    public:
        SkyLayer();

        void setup();
        void update();
        void draw(glm::mat4 view, glm::mat4 projection, camera_t camera);
        void teardown();

        void receive_resource(ResourceType type, string name, void *data);
};