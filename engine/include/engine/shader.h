#pragma once

#include "layer.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <map>
#include <string>
#include <vector>

using std::string;
using std::vector;

class Shader {
private:
    string vertex_path, fragment_path;
    std::map<string, GLuint> uniform_locations;
    std::map<string, GLint> attrib_locations;
public:
    Shader(string vertex_path, string fragment_path);

    unsigned int id;
    vector<Layer*> layers;

    void setup();
    void compile();
    void use();
    void update();
    void teardown();
    void set_bool(const string name, bool value) const;
    void set_int(const string name, int value) const;
    void set_float(const string name, float value) const;
    void add_layer(Layer *layer);

    GLuint get_uniform_location(string name);
    GLint get_attrib_location(string name);
};
