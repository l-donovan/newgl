#pragma once

// Forward declarations
class Material;
class Shader;

#include <glm/glm.hpp>

#include <plog/Log.h>

#include "global.h"
#include "attribute.h"
#include "common.h"
#include "shader.h"
#include "mesh.h"

#include <memory>
#include <optional>
#include <queue>
#include <string>
#include <vector>

using std::string;

class Layer {
private:
    std::queue<resource_request_t> request_queue;
protected:
    std::vector<std::shared_ptr<Attribute>> attributes;
    bool draw_with_depth = true;
public:
    Shader *shader;
    Material *material = nullptr;

    virtual void setup() = 0;
    virtual void draw(glm::mat4 view, glm::mat4 projection, camera_t camera) = 0;
    virtual void update() = 0;
    virtual void teardown() = 0;

    void request_resource(ResourceType type, string name) {
        PLOGD << "Requesting resource \"" << name << "\" of type " << type;
        this->request_queue.push({type, name});
    };

    virtual void receive_resource(ResourceType type, string name, void *data) = 0;

    std::optional<resource_request_t> pop_resource_request() {
        if (this->request_queue.empty())
            return {};

        resource_request_t request = this->request_queue.front();
        this->request_queue.pop();
        return request;
    };

    std::vector<std::shared_ptr<Attribute>> get_attributes() {
        return this->attributes;
    };

    void add_attribute(std::shared_ptr<Attribute> attr) {
        this->attributes.push_back(attr);
    };

    bool depth_enabled() {
        return this->draw_with_depth;
    };
};
