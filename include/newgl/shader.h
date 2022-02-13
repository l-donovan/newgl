#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <string>
#include <vector>

#include "layer.h"

using std::string;
using std::vector;

class Shader {
    private:
        vector<Layer*> layers;
        string vertex_path, fragment_path;
    public:
        Shader(string vertex_path, string fragment_path);

        unsigned int id;

        void setup();
        void compile();
        void use();
        void update();
        void teardown();
        void set_bool(const string name, bool value) const;
        void set_int(const string name, int value) const;
        void set_float(const string name, float value) const;
        void add_layer(Layer *layer);
        void draw_layers();
};
