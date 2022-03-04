#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <plog/Log.h>

#include "engine/layers/entity_layer.h"
#include "engine/global.h"
#include "engine/common.h"
#include "engine/mesh.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>

using std::string;

EntityLayer::EntityLayer() {}

void EntityLayer::setup() {
    glGenBuffers(1, &this->vbo_vertices);
    glGenBuffers(1, &this->vbo_normals);
    glGenBuffers(1, &this->vbo_uvs);
    glGenBuffers(1, &this->ibo_faces);

    this->model_location = this->shader->get_uniform_location("model");
    this->view_location = this->shader->get_uniform_location("view");
    this->projection_location = this->shader->get_uniform_location("projection");
    this->camera_location = this->shader->get_uniform_location("camera");
    this->texture_location = this->shader->get_uniform_location("texture_0");

    this->vertex_location = this->shader->get_attrib_location("vertex");
    this->uv_location = this->shader->get_attrib_location("uv");
    this->normal_location = this->shader->get_attrib_location("normal");
}

void EntityLayer::update() {
    this->calculate_attribute_buffers();
}

void EntityLayer::receive_resource(ResourceType type, string name, void *data) {
    if (type == MeshResource) {
        Mesh *mesh = (Mesh*) data;

        this->vert_count = mesh->vertices.size();
        this->uv_count = mesh->uvs.size();
        this->normal_count = mesh->normals.size();
        this->tri_count = mesh->faces.size();

        this->vertices = &(mesh->vertices)[0].x;
        this->uvs = &(mesh->uvs)[0].x;
        this->normals = &(mesh->normals)[0].x;
        this->faces = &(mesh->faces)[0];

        this->calculate_attribute_buffers();
    } else if (type == Texture) {
        GLuint *tex = (GLuint*) data;

        this->texture_0 = *tex;
    }
}

void EntityLayer::teardown() {
    glDeleteBuffers(1, &this->vbo_vertices);
    glDeleteBuffers(1, &this->vbo_normals);
    glDeleteBuffers(1, &this->vbo_uvs);
    glDeleteBuffers(1, &this->ibo_faces);

    glDeleteTextures(1, &this->texture_0);
}

void EntityLayer::set_position(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
}


void EntityLayer::set_scale(float x, float y, float z) {
    if (y == 0.0f || z == 0.0f)
        y = z = x;

    this->sx = x;
    this->sy = y;
    this->sz = z;
}

void EntityLayer::calculate_attribute_buffers() {
    PLOGD << "Calculating attribute buffers";

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vertices);
    glBufferData(
        GL_ARRAY_BUFFER,
        4 * this->vert_count * sizeof(float),
        this->vertices,
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_uvs);
    glBufferData(
        GL_ARRAY_BUFFER,
        2 * this->uv_count * sizeof(float),
        this->uvs,
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_normals);
    glBufferData(
        GL_ARRAY_BUFFER,
        3 * this->normal_count * sizeof(float),
        this->normals,
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo_faces);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        this->tri_count * sizeof(GLushort),
        this->faces,
        GL_STATIC_DRAW
    );
}

void EntityLayer::draw(glm::mat4 view, glm::mat4 projection, camera_t camera) {
    glm::vec3 translation(this->x, this->y, this->z);

    glm::mat4 model = glm::mat4(1.0);
    model = glm::translate(model, translation);
    model = glm::scale(model, glm::vec3(this->sx, this->sy, this->sz));

    glUniformMatrix4fv(this->model_location, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(this->view_location, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(this->projection_location, 1, GL_FALSE, glm::value_ptr(projection));

    glUniform3fv(this->camera_location, 1, glm::value_ptr(camera.position));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->texture_0);
    glUniform1i(this->texture_location, 0);

    glEnableVertexAttribArray(this->vertex_location);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vertices);
    glVertexAttribPointer(this->vertex_location, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(this->uv_location);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_uvs);
    glVertexAttribPointer(this->uv_location, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(this->normal_location);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_normals);
    glVertexAttribPointer(this->normal_location, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo_faces);
    glDrawElements(GL_TRIANGLES, this->tri_count, GL_UNSIGNED_SHORT, 0);

    glDisableVertexAttribArray(this->vertex_location);
    glDisableVertexAttribArray(this->uv_location);
    glDisableVertexAttribArray(this->normal_location);
}
