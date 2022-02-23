#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "engine/common.h"
#include "engine/example_layer.h"

#include <cmath>
#include <iostream>
#include <vector>

void ExampleLayer::setup() {
    std::vector<glm::vec3> vertices;
    vertices.push_back(glm::vec3(-1.0, -1.0f, 0.0f));
    vertices.push_back(glm::vec3(1.0f, -1.0f, 0.0f));
    vertices.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

    std::vector<glm::vec3> colors;
    colors.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
    colors.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    colors.push_back(glm::vec3(0.0f, 0.0f, 1.0f));

    std::vector<GLushort> faces;
    faces.push_back(0);
    faces.push_back(1);
    faces.push_back(2);

    glGenBuffers(1, &this->vbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &this->vbo_colors);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_colors);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &this->ibo_faces);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo_faces);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(GLushort), faces.data(), GL_STATIC_DRAW);
}

void ExampleLayer::draw(glm::mat4 view, glm::mat4 projection, camera_t camera) {
    // update the uniform color
    //float time_val = glfwGetTime();
    //float green_val = sin(time_val) / 2.0f + 0.5f;
    //int uniform_location = glGetUniformLocation(this->shader_id, "myThing");
    //glUniform3f(uniform_location, 1.0f, green_val, 0.0f);

    GLint vertex_position = this->shader->get_attrib_location("vertex");
    glEnableVertexAttribArray(vertex_position);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vertices);
    glVertexAttribPointer(vertex_position, 3, GL_FLOAT, GL_FALSE, 0, 0);

    GLint color_position = this->shader->get_attrib_location("color");
    glEnableVertexAttribArray(color_position);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_colors);
    glVertexAttribPointer(color_position, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo_faces);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);

    glDisableVertexAttribArray(vertex_position);
    glDisableVertexAttribArray(color_position);
}

void ExampleLayer::update() {
}

void ExampleLayer::teardown() {
    glDeleteBuffers(1, &this->vbo_vertices);
    glDeleteBuffers(1, &this->vbo_colors);
    glDeleteBuffers(1, &this->ibo_faces);
}
