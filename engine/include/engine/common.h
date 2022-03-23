#pragma once

#include <glm/glm.hpp>

#include <cstdlib>
#include <cstring>
#include <iostream>
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

#define ADD_ATTRIBUTE(layer, attr, ...) layer.add_attribute(std::make_shared<attr>(&layer, __VA_ARGS__))
#define ADD_GENERIC_ATTRIBUTE(layer, attr, ...) layer.add_attribute(std::make_shared<attr>(__VA_ARGS__))

#define CONTAINS(iterable, val) iterable.find(val) != iterable.end()

#define CAT(x, y) CAT_(x, y)
#define CAT_(x, y) x ## y
#define GENERATE_STRING(STRING) #STRING,
#define ENUM_NAME(NAME, VAL) CAT(NAME, _names)[VAL]

#define MAX_ENUM_NAME_LENGTH 100

static char **extract_names(const char* name_str) {
    char **names;
    char tmp[MAX_ENUM_NAME_LENGTH];
    char c;

    int index_in_str = 0;
    int index_in_name = 0;
    int name_num = 0;

    int count = 1;

    while ((c = name_str[index_in_str++]) != 0) {
        if (c == ',') {
            ++count;
        }
    }

    names = (char**) malloc(count * sizeof(char*));

    index_in_str = 0;

    while ((c = name_str[index_in_str++]) != 0) {
        if (c == ',') {
            tmp[index_in_name] = 0;
            names[name_num] = (char*) malloc((index_in_name + 1) * sizeof(char));
            memcpy(names[name_num], tmp, (index_in_name + 1) * sizeof(char));
            index_in_name = 0;
            ++name_num;
            ++index_in_str;
        } else {
            tmp[index_in_name] = c;
            ++index_in_name;
        }
    }

    tmp[index_in_name] = 0;
    names[name_num] = (char*) malloc((index_in_name + 1) * sizeof(char));
    memcpy(names[name_num], tmp, (index_in_name + 1) * sizeof(char));

    return names;
}

#define GENERATE_ENUM_WITH_NAMES(NAME, ...) \
    enum NAME { __VA_ARGS__ }; \
    static char **CAT(NAME, _names) = extract_names(#__VA_ARGS__);

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
