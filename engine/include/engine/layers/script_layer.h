#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>

#include "engine/common.h"
#include "engine/global.h"
#include "engine/layer.h"

#include <string>

using std::string;

class ScriptLayer : public Layer {
    public:
        ScriptLayer();

        void setup();
        void update();
        void draw(glm::mat4 view, glm::mat4 projection, camera_t camera);
        void teardown();

        void receive_resource(ResourceType type, string name, void *data);
};