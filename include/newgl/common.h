#pragma once

#include <glm/glm.hpp>

#include <string>

using std::string;

#define INT(E, X)    std::get<int>((E).data[X])
#define FLOAT(E, X)  std::get<float>((E).data[X])
#define DOUBLE(E, X) std::get<double>((E).data[X])
#define LONG(E, X)   std::get<long>((E).data[X])
#define LAYER(E, X)  std::get<Layer*>((E).data[X])
#define SHADER(E, X) std::get<Shader*>((E).data[X])
#define STRING(E, X) std::get<string>((E).data[X])
#define VOID(E, X)   std::get<void*>((E).data[X])

struct camera_t {
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
    float vfov;
};

enum EventType {
    WindowResize,
    Key,
    CursorPosition,
    MeshLoad,
    TextureLoad,
    Framerate,

    WindowResizeRequest,
    LayerUpdateRequest,
    BufferModifyRequest,
    LayerModifyRequest,
    MeshLoadRequest,
    TextureLoadRequest,

    // Right now, ticks are just frames, but who's counting?
    Tick1,
    Tick10,
    Tick100,
};

enum ResourceType {
    Mesh,
    Texture,
};

struct resource_request_t {
    enum ResourceType type;
    string name;
};
