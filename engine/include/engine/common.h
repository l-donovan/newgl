#pragma once

#include <glm/glm.hpp>

#include "engine/map.h"

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

#define ADD_LAYER(layer, shader) this->outgoing_events.enqueue({EventType::LayerModifyRequest, {EVENT_LAYER_ADD, &layer, &shader}})
#define ADD_BLANK_LAYER(layer) this->outgoing_events.enqueue({EventType::LayerModifyRequest, {EVENT_LAYER_ADD_BLANK, &layer}})
#define REMOVE_LAYER(layer) this->outgoing_events.enqueue({EventType::LayerModifyRequest, {EVENT_LAYER_REMOVE, &layer}})

#define CONTAINS(iterable, val) iterable.find(val) != iterable.end()

#define CAT(x, y) CAT_(x, y)
#define CAT_(x, y) x ## y
#define GENERATE_STRING(STRING) #STRING,
#define NUMARGS(...)  (sizeof((int[]){__VA_ARGS__})/sizeof(int))
#define ENUM_NAME(NAME, VAL) CAT(NAME, _names)[VAL]
#define ENUM_FIRST(NAME) CAT(NAME, _first)
#define ENUM_LAST(NAME) CAT(NAME, _last)

#define GENERATE_ENUM_WITH_NAMES(NAME, ...) \
    enum NAME { __VA_ARGS__ }; \
    static const char *CAT(NAME, _names)[] = { MAP(GENERATE_STRING, __VA_ARGS__) }; \
    static int CAT(NAME, _first) = ((int[]){__VA_ARGS__})[0]; \
    static int CAT(NAME, _last) = NUMARGS(__VA_ARGS__) - 1;

GENERATE_ENUM_WITH_NAMES(ResourceType,
    MeshResource,
    Texture
)

GENERATE_ENUM_WITH_NAMES(InputEventType,
    CursorPositionInput,
    KeyInput,
    MouseButtonInput,
    ScrollInput
)

struct camera_t {
    glm::vec3 position;
    glm::mat4 view;
    glm::mat4 projection;
    float yaw;
    float pitch;
    float vfov;
};

struct resource_request_t {
    enum ResourceType type;
    string name;
};
