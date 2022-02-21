#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>

#include "common.h"
#include "layer.h"
#include "mesh.h"

#include <string>

using std::string;

class EntityLayer : public Layer {
    private:
        // VBOs and IBOs
        GLuint vbo_vertices = 0;
        GLuint vbo_uvs = 0;
        GLuint vbo_normals = 0;
        GLuint ibo_faces = 0;

        // Layer positioning
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float sx = 1.0f;
        float sy = 1.0f;
        float sz = 1.0f;

        // Buffers
        float *vertices = nullptr;
        float *uvs = nullptr;
        float *normals = nullptr;
        GLushort *faces = nullptr;

        // Textures
        GLuint texture_0 = 0;

        // Locations
        GLuint model_location = 0;
        GLuint view_location = 0;
        GLuint projection_location = 0;
        GLuint camera_location = 0;
        GLuint texture_location = 0;
        GLint vertex_location = 0;
        GLint uv_location = 0;
        GLint normal_location = 0;

        GLuint vert_count = 0;
        GLuint uv_count = 0;
        GLuint normal_count = 0;
        GLuint tri_count = 0;
    public:
        EntityLayer();

        void setup();
        void update();
        void draw(glm::mat4 view, glm::mat4 projection, camera_t camera);
        void teardown();

        void load_mesh(string mesh_filename);
        void receive_mesh(Mesh mesh);
        void set_position(float x, float y, float z);
        void set_scale(float x, float y = 0.0f, float z = 0.0f);
        void calculate_attribute_buffers();

        void receive_resource(ResourceType type, string name, void *data);
};
