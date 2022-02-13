#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

#include "newgl/global.h"
#include "newgl/common.h"
#include "newgl/layer.h"
#include "newgl/shader.h"
#include "newgl/window.h"

using std::string;

Shader::Shader(string vertex_path, string fragment_path) {
    this->vertex_path = vertex_path;
    this->fragment_path = fragment_path;
}

void Shader::setup() {
    this->compile();

    for (Layer *layer : this->layers) {
        layer->shader_id = this->id;
        layer->setup();
    }
}

void Shader::compile() {
    // Load shaders
    string vertex_code;
    string fragment_code;

    std::ifstream vertex_file;
    std::ifstream fragment_file;

    vertex_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fragment_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        // Open files
        vertex_file.open(this->vertex_path);
        fragment_file.open(this->fragment_path);
        std::stringstream vertex_stream, fragment_stream;

        // Read file's buffer contents into streams
        vertex_stream << "#version 410" << std::endl << vertex_file.rdbuf();
        fragment_stream << "#version 410" << std::endl << fragment_file.rdbuf();

        // Close file handlers
        vertex_file.close();
        fragment_file.close();

        // Convert stream into string
        vertex_code = vertex_stream.str();
        fragment_code = fragment_stream.str();
    } catch (std::ifstream::failure e) {
        PLOGE << "Failed to read shader file";
    }

    const char* vertex_shader = vertex_code.c_str();
    const char* fragment_shader = fragment_code.c_str();

    // Compile shaders
    unsigned int vertex, fragment;
    int success;
    char info_log[512];

    // Vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_shader, NULL);
    glCompileShader(vertex);

    // Print compile errors, if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, info_log);
        PLOGE << "Failed to compile vertex shader: " << info_log;
    }

    // Fragment shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragment_shader, NULL);
    glCompileShader(fragment);

    // Print compile errors, if any
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, info_log);
        PLOGE << "Failed to compile fragment shader: " << info_log;
    }

    // Shader Program
    this->id = glCreateProgram();
    glAttachShader(this->id, vertex);
    glAttachShader(this->id, fragment);
    glLinkProgram(this->id);

    // Print linking errors, if any
    glGetProgramiv(this->id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(id, 512, NULL, info_log);
        PLOGE << "Failed to link shader program: " << info_log;
    }

    // Delete the shaders. They're linked into our program now and are no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use() {
    glUseProgram(this->id);
}

void Shader::update() {
    for (Layer *layer : layers) {
        layer->update();
    }
}

void Shader::teardown() {
    for (Layer *layer : layers) {
        layer->teardown();
    }

    glDeleteProgram(this->id);
}

void Shader::set_bool(const string name, bool value) const {
    glUniform1i(glGetUniformLocation(this->id, name.c_str()), (int) value);
}

void Shader::set_int(const string name, int value) const {
    glUniform1i(glGetUniformLocation(this->id, name.c_str()), value);
}

void Shader::set_float(const string name, float value) const {
    glUniform1f(glGetUniformLocation(this->id, name.c_str()), value);
}

void Shader::add_layer(Layer *layer) {
    // TODO: Can probably make this into an unordered_set or something faster
    if (std::find(this->layers.begin(), this->layers.end(), layer) == this->layers.end()) {
        this->layers.push_back(layer);
    }
}

void Shader::draw_layers() {
    struct camera_t camera = {
        .position = glm::vec3(0.0, 2.0, 4.0),
        .target   = glm::vec3(0.0, 0.0, 0.0),
        .up       = glm::vec3(0.0, 1.0, 0.0),
        .vfov     = 45.0
    };

    float ratio = 1.0f * Window::width / Window::height;
    glm::mat4 view = glm::lookAt(camera.position, camera.target, camera.up);
    glm::mat4 projection = glm::perspective(camera.vfov, ratio, 0.1f, 100.0f);

    for (Layer *layer : this->layers) {
        layer->draw(view, projection, camera);
    }
}
