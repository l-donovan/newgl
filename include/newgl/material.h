#pragma once

#include "global.h"
#include "shader.h"

#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>

#include <map>
#include <string>
#include <variant>

using std::string;

using uniform_variant_t = std::variant<
    int, float, double, glm::vec2, glm::vec3, glm::vec4>;

class Material {
    private:
        std::map<string, uniform_variant_t> uniform_values = {};
    public:
        Material() {};

        void set(string name, uniform_variant_t value) {
            this->uniform_values[name] = value;
        };

        void apply(Shader *shader) {
            for (auto it = this->uniform_values.begin(); it != this->uniform_values.end(); ++it) {
                GLint loc = shader->get_uniform_location("mat_" + it->first);

                if (std::holds_alternative<int>(it->second)) {
                    glUniform1i(loc, std::get<int>(it->second));
                } else if (std::holds_alternative<float>(it->second)) {
                    glUniform1f(loc, std::get<float>(it->second));
                } else if (std::holds_alternative<double>(it->second)) {
                    glUniform1d(loc, std::get<double>(it->second));
                } else if (std::holds_alternative<glm::vec2>(it->second)) {
                    glUniform2fv(loc, 1, glm::value_ptr(std::get<glm::vec2>(it->second)));
                } else if (std::holds_alternative<glm::vec3>(it->second)) {
                    glUniform3fv(loc, 1, glm::value_ptr(std::get<glm::vec3>(it->second)));
                } else if (std::holds_alternative<glm::vec4>(it->second)) {
                    glUniform4fv(loc, 1, glm::value_ptr(std::get<glm::vec4>(it->second)));
                }
            }
        };
};
