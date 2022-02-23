#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <plog/Log.h>

#include "engine/common.h"
#include "engine/global.h"
#include "engine/sky_layer.h"

using std::string;

SkyLayer::SkyLayer() {
    this->draw_with_depth = false;
}

void SkyLayer::setup() {
    glGenBuffers(1, &this->vbo_vertices);
    glGenBuffers(1, &this->ibo_faces);

    this->view_location = this->shader->get_uniform_location("view");
    this->projection_location = this->shader->get_uniform_location("projection");
    this->texture_location = this->shader->get_uniform_location("texture_0");

    this->vertex_location = this->shader->get_attrib_location("vertex");

    float vertices[] = {
        -1, -1, 0,
         1, -1, 0,
         1,  1, 0,
        -1,  1, 0
    };

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vertices);
    glBufferData(
        GL_ARRAY_BUFFER,
        4 * 3 * sizeof(float),
        vertices,
        GL_STATIC_DRAW
    );

    GLushort faces[] = {
        0, 1, 2,
        0, 2, 3
    };

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo_faces);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        2 * 3 * sizeof(GLushort),
        faces,
        GL_STATIC_DRAW
    );
}

void SkyLayer::update() {}

void SkyLayer::receive_resource(ResourceType type, string name, void *data) {
    if (type == Texture) {
        GLuint *tex = (GLuint*) data;
        this->texture_0 = *tex;
    }
}

void SkyLayer::teardown() {
    glDeleteBuffers(1, &this->vbo_vertices);
    glDeleteBuffers(1, &this->ibo_faces);

    glDeleteTextures(1, &this->texture_0);
}

void SkyLayer::draw(glm::mat4 view, glm::mat4 projection, camera_t camera) {
    glUniformMatrix4fv(this->view_location, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(this->projection_location, 1, GL_FALSE, glm::value_ptr(projection));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->texture_0);
    glUniform1i(this->texture_location, 0);

    glEnableVertexAttribArray(this->vertex_location);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vertices);
    glVertexAttribPointer(this->vertex_location, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo_faces);
    glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_SHORT, 0);

    glDisableVertexAttribArray(this->vertex_location);
}