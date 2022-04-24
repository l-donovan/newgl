#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>

#include <plog/Log.h>

#include "engine/layers/entity_layer.h"
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

    this->position = glm::vec3(this->x, this->y, this->z) + this->center;
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

float distance_from_center;
float distance_from_enclosing_sphere;
float planar_distance_from_camera;
float angle;
float angle_threshold;

void EntityLayer::draw(glm::mat4 view, glm::mat4 projection, camera_t camera) {
    // Before doing any expensive rendering, see if we can skip drawing the entity altogether
    // by checking if all of the sphere enclosing its bounding box is outside our FOV.

    glm::vec3 camera_direction(-sinf(camera.yaw), sinf(camera.pitch), -cosf(camera.yaw)); // TODO: Can be saved in camera_t
    camera_direction = glm::normalize(camera_direction); // TODO: See above

    // We find the distance from the the center of the mesh to the line formed by the camera's position and direction.
    // Then we can subtract the radius to find the distance to the enclosing sphere.
    // NOTE: Adapted from https://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html
    distance_from_center = glm::length(glm::cross(camera_direction, camera.position - this->position));
    distance_from_enclosing_sphere = distance_from_center - this->enclosing_sphere_radius;

    // We imagine a plane defined using the camera's direction as its normal. We then compute the distance
    // the plane must be along the camera's direction vector for it to be coplanar with the mesh's center.
    // NOTE: Adapted from https://en.wikibooks.org/wiki/Linear_Algebra/Orthogonal_Projection_Onto_a_Line
    glm::vec3 proj = glm::dot(this->position - camera.position, camera_direction) * camera_direction;
    planar_distance_from_camera = glm::length(proj);
    angle = abs(atan2f(distance_from_enclosing_sphere, planar_distance_from_camera));

    // Because our screen (probably) isn't circular, we'll have blind spots in the corners of the
    // screen if we use the camera's VFOV as the threshold value. Instead we circumscribe a circle
    // around the screen and treat that as our FOV threshold.
    angle_threshold = glm::radians(camera.vfov) * Application::width / Application::height;

    // Finally, skip rendering the entity if the angle is greater than our calculated threshold.
    if (angle > angle_threshold) {
        return;
    }

    glm::mat4 model = this->transform;
    model = glm::translate(model, this->position);
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
