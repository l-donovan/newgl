#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>

#include <string>
#include <vector>

using std::string;
using std::vector;

class Mesh {
    public:
        Mesh() {};

        vector<glm::vec4> vertices;
        vector<glm::vec2> uvs;
        vector<glm::vec3> normals;
        vector<GLushort> faces;

        static Mesh from_obj(string filename);
};