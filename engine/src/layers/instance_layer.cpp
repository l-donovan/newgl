#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>

#include <plog/Log.h>

#include "engine/layers/instance_layer.h"
#include "engine/application.h"
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

void InstanceLayer::setup() {
    glGenBuffers(1, &this->vbo_models);
    glGenBuffers(1, &this->vbo_vertices);
    glGenBuffers(1, &this->vbo_normals);
    glGenBuffers(1, &this->vbo_uvs);
    glGenBuffers(1, &this->ibo_faces);
    glGenVertexArrays(1, &this->vao);

    this->view_location = this->shader->get_uniform_location("view");
    this->projection_location = this->shader->get_uniform_location("projection");
    this->camera_location = this->shader->get_uniform_location("camera");
    this->texture_location = this->shader->get_uniform_location("texture_0");

    GLint m = this->shader->get_attrib_location("model");
    this->model_locations[0] = m + 0;
    this->model_locations[1] = m + 1;
    this->model_locations[2] = m + 2;
    this->model_locations[3] = m + 3;
    this->vertex_location = this->shader->get_attrib_location("vertex");
    this->uv_location = this->shader->get_attrib_location("uv");
    this->normal_location = this->shader->get_attrib_location("normal");
}

void InstanceLayer::update() {
    this->calculate_attribute_buffers();
}

void InstanceLayer::receive_resource(ResourceType type, string name, void *data) {
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

        float min_x, max_x, min_y, max_y, min_z, max_z;

        min_x = max_x = mesh->vertices[0].x;
        min_y = max_y = mesh->vertices[0].y;
        min_z = max_z = mesh->vertices[0].z;

        for (int i = 0; i < mesh->vertices.size(); ++i) {
            if (mesh->vertices[i].x < min_x) min_x = mesh->vertices[i].x;
            if (mesh->vertices[i].x > max_x) max_x = mesh->vertices[i].x;
            if (mesh->vertices[i].y < min_y) min_y = mesh->vertices[i].y;
            if (mesh->vertices[i].y > max_y) max_y = mesh->vertices[i].y;
            if (mesh->vertices[i].z < min_z) min_z = mesh->vertices[i].z;
            if (mesh->vertices[i].z > max_z) max_z = mesh->vertices[i].z;
        }

        glm::vec3 size = glm::vec3(max_x - min_x, max_y - min_y, max_z - min_z);
        this->enclosing_sphere_radius = glm::length(size) / 2.0f;
        this->center = glm::vec3((min_x + max_x) / 2.0f, (min_y + max_y) / 2.0f, (min_z + max_z) / 2.0f);
        this->transform = glm::translate(glm::mat4(1), center); // TODO: Not implemented
        this->position = glm::vec3(this->x, this->y, this->z) + this->center;

        _models = new glm::mat4[this->instance_count];
        glm::mat4 base_model = this->transform;
        base_model = glm::translate(base_model, this->position);
        base_model = glm::scale(base_model, glm::vec3(this->sx, this->sy, this->sz));

        for (int i = 0; i < this->instance_count; ++i) {
            _models[i] = this->instance_func(base_model, i);
        }

        this->models = (float*)&_models[0];

        this->calculate_attribute_buffers();
    } else if (type == Texture) {
        GLuint *tex = (GLuint*) data;

        this->texture_0 = *tex;
    }
}

void InstanceLayer::teardown() {
    glDeleteBuffers(1, &this->vbo_vertices);
    glDeleteBuffers(1, &this->vbo_normals);
    glDeleteBuffers(1, &this->vbo_uvs);
    glDeleteBuffers(1, &this->ibo_faces);

    glDeleteTextures(1, &this->texture_0);

    glDeleteVertexArrays(1, &this->vao);
}

void InstanceLayer::set_position(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;

    this->position = glm::vec3(this->x, this->y, this->z) + this->center;
}

void InstanceLayer::set_scale(float x, float y, float z) {
    if (y == 0.0f || z == 0.0f)
        y = z = x;

    this->sx = x;
    this->sy = y;
    this->sz = z;
}

void InstanceLayer::calculate_attribute_buffers() {
    PLOGD << "Calculating attribute buffers";

    glBindVertexArray(this->vao);

    // Vertices
    glEnableVertexAttribArray(this->vertex_location);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vertices);
    glBufferData(
        GL_ARRAY_BUFFER,
        4 * this->vert_count * sizeof(float),
        this->vertices,
        GL_STATIC_DRAW
    );
    glVertexAttribPointer(this->vertex_location, 4, GL_FLOAT, GL_FALSE, 0, 0);

    // UVs
    glEnableVertexAttribArray(this->uv_location);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_uvs);
    glBufferData(
        GL_ARRAY_BUFFER,
        2 * this->uv_count * sizeof(float),
        this->uvs,
        GL_STATIC_DRAW
    );
    glVertexAttribPointer(this->uv_location, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Normals
    glEnableVertexAttribArray(this->normal_location);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_normals);
    glBufferData(
        GL_ARRAY_BUFFER,
        3 * this->normal_count * sizeof(float),
        this->normals,
        GL_STATIC_DRAW
    );
    glVertexAttribPointer(this->normal_location, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Model matrices
    glEnableVertexAttribArray(this->model_locations[0]);
    glEnableVertexAttribArray(this->model_locations[1]);
    glEnableVertexAttribArray(this->model_locations[2]);
    glEnableVertexAttribArray(this->model_locations[3]);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_models);
    glBufferData(
        GL_ARRAY_BUFFER,
        this->instance_count * sizeof(glm::mat4),
        this->models,
        GL_STATIC_DRAW
    );
    glVertexAttribPointer(this->model_locations[0], 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(0));
    glVertexAttribPointer(this->model_locations[1], 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 4));
    glVertexAttribPointer(this->model_locations[2], 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 8));
    glVertexAttribPointer(this->model_locations[3], 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 12));
    glVertexAttribDivisor(this->model_locations[0], 1);
    glVertexAttribDivisor(this->model_locations[1], 1);
    glVertexAttribDivisor(this->model_locations[2], 1);
    glVertexAttribDivisor(this->model_locations[3], 1);

    // Faces
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo_faces);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        this->tri_count * sizeof(GLushort),
        this->faces,
        GL_STATIC_DRAW
    );

    glBindVertexArray(0);
}

void InstanceLayer::draw(glm::mat4 view, glm::mat4 projection, camera_t camera) {
    glUniformMatrix4fv(this->view_location, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(this->projection_location, 1, GL_FALSE, glm::value_ptr(projection));

    glUniform3fv(this->camera_location, 1, glm::value_ptr(camera.position));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->texture_0);
    glUniform1i(this->texture_location, 0);

    glBindVertexArray(this->vao);
    glDrawElementsInstanced(GL_TRIANGLES, this->tri_count, GL_UNSIGNED_SHORT, 0, this->instance_count);
    glBindVertexArray(0);
}
