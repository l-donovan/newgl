#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>

#include <vector>

using std::vector;

struct mesh_t {
    vector<glm::vec4> *vertices;
    vector<glm::vec2> *uvs;
    vector<glm::vec3> *normals;
    vector<GLushort> *faces;
};

bool load_obj(const char *filename, mesh_t *mesh);
struct attributes* generate_attributes(GLfloat **coord_3d, GLfloat **v_color, GLfloat **coord_uv);
