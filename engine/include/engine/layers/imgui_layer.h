#pragma once

#include <glm/glm.hpp>

#include "engine/layer.h"
#include "engine/common.h"
#include "engine/global.h"

#include <string>

using std::string;

class ImguiLayer : public Layer {
    private:
    public:
        ImguiLayer();

        void setup();
        void update();
        void draw(glm::mat4 view, glm::mat4 projection, camera_t camera);
        void teardown();

        void receive_resource(ResourceType type, string name, void *data);
};