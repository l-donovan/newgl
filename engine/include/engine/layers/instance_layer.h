#pragma once

#include "engine/common.h"
#include "engine/global.h"
#include "engine/layer.h"

#include <string>
#include <type_traits>

using std::string;

typedef bool (*InstanceFunction)(int num);

template <class T>
class InstanceLayer : public Layer {
    static_assert(std::is_base_of<Layer, T>::value, "T must inherit from list");
private:
    T *layer;
    int size;
    InstanceFunction instance_func;
public:
    InstanceLayer(int size, T *target, InstanceFunction func) : size(size), layer(target), instance_func(func) {};

    void setup() {
        this->layer->shader = this->shader;
        this->layer->setup();
    };

    void update() {
        this->layer->update();
    };

    void draw(glm::mat4 view, glm::mat4 projection, camera_t camera) {
        for (int i = 0; i < this->size; ++i) {
            if (this->instance_func(i)) {
                this->layer->draw(view, projection, camera);
            }
        }
    };

    void teardown() {
        this->layer->teardown();
    };

    void set_scale(int size) {
        this->size = size;
    };

    void receive_resource(ResourceType type, string name, void *data) {
        this->layer->receive_resource(type, name, data);
    };
};