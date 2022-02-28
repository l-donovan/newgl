#pragma once

#include <glm/glm.hpp>

#include <string>

using std::string;

#define INT(X)     std::get<int>(event.data[X])
#define FLOAT(X)   std::get<float>(event.data[X])
#define DOUBLE(X)  std::get<double>(event.data[X])
#define LONG(X)    std::get<long>(event.data[X])
#define BOOL(X)    std::get<bool>(event.data[X])
#define STRING(X)  std::get<string>(event.data[X])
#define LAYER(X)   std::get<Layer*>(event.data[X])
#define SHADER(X)  std::get<Shader*>(event.data[X])
#define VOID(X)    std::get<void*>(event.data[X])
#define SURFACE(X) std::get<SDL_Surface*>(event.data[X])
#define MESH(X)    std::get<Mesh*>(event.data[X])

#define ADD_ATTRIBUTE(layer, attr, ...) layer.add_attribute(std::make_shared<attr>(&layer __VA_OPT__(, ) __VA_ARGS__))
#define ADD_GENERIC_ATTRIBUTE(layer, attr, ...) layer.add_attribute(std::make_shared<attr>(__VA_ARGS__))

struct camera_t {
    glm::vec3 position;
    glm::mat4 view;
    glm::mat4 projection;
    float yaw;
    float pitch;
    float vfov;
};

enum ResourceType {
    MeshResource,
    Texture,
};

struct resource_request_t {
    enum ResourceType type;
    string name;
};
