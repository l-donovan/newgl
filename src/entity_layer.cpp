#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <SDL2/SDL.h>
#include <SDL_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <plog/Log.h>

#include "newgl/global.h"
#include "newgl/common.h"
#include "newgl/entity_layer.h"
#include "newgl/mesh.h"
#include "newgl/window.h"

#include "newgl/attributes/test.h"

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

EntityLayer::EntityLayer() {
    this->attributes.push_back(std::make_shared<Test>(this));
}

void EntityLayer::setup() {
    glGenBuffers(1, &this->vbo_vertices);
    glGenBuffers(1, &this->vbo_normals);
    glGenBuffers(1, &this->vbo_uvs);
    glGenBuffers(1, &this->ibo_faces);

    this->request_resource(Texture, "resources/images/chinese_garden.png");
    this->request_resource(Mesh, "resources/models/suzanne.obj");

    this->model_location = glGetUniformLocation(this->shader_id, "model");
    this->view_location = glGetUniformLocation(this->shader_id, "view");
    this->projection_location = glGetUniformLocation(this->shader_id, "projection");
    this->camera_location = glGetUniformLocation(this->shader_id, "camera");
    this->texture_location = glGetUniformLocation(this->shader_id, "texture_0");

    this->vertex_location = glGetAttribLocation(this->shader_id, "vertex");
    this->uv_location = glGetAttribLocation(this->shader_id, "uv");
    this->normal_location = glGetAttribLocation(this->shader_id, "normal");
}

void EntityLayer::update() {
    //this->calculate_attribute_buffers();
}

void EntityLayer::receive_resource(ResourceType type, string name, void *data) {
    if (type == Mesh) {
        mesh_t *mesh = (mesh_t*) data;

        this->vert_count = mesh->vertices->size();
        this->uv_count = mesh->uvs->size();
        this->normal_count = mesh->normals->size();
        this->tri_count = mesh->faces->size();

        this->vertices = &(*mesh->vertices)[0].x;
        this->uvs = &(*mesh->uvs)[0].x;
        this->normals = &(*mesh->normals)[0].x;
        this->faces = &(*mesh->faces)[0];

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

void EntityLayer::calculate_attribute_buffers() {
    PLOGD << "Calculating attribute buffers."
          << " This should only happen when mesh geometry is manipulated";

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
    model = glm::scale(model, glm::vec3(1.0));

    glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(projection));

    glUniform3fv(camera_location, 1, glm::value_ptr(camera.position));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->texture_0);
    glUniform1i(texture_location, 0);

    glEnableVertexAttribArray(vertex_location);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vertices);
    glVertexAttribPointer(vertex_location, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(uv_location);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_uvs);
    glVertexAttribPointer(uv_location, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(normal_location);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_normals);
    glVertexAttribPointer(normal_location, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo_faces);
    glDrawElements(GL_TRIANGLES, this->tri_count, GL_UNSIGNED_SHORT, 0);

    glDisableVertexAttribArray(vertex_location);
    glDisableVertexAttribArray(uv_location);
    glDisableVertexAttribArray(normal_location);
}
