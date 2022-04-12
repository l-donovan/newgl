#pragma once

#include "global.h"
#include "shader.h"

#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>

#include <map>
#include <string>
#include <variant>

// TODO: Remove
#include <iostream>

using std::string;

using uniform_variant_t = std::variant<
    int, float, double, glm::vec2, glm::vec3, glm::vec4>;

class Material {
private:
    std::map<string, uniform_variant_t> uniform_values;
public:
    Material();

    void set(string name, uniform_variant_t value);
    void apply(Shader *shader);
};
