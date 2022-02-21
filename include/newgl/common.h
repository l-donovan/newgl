#pragma once

#include <glm/glm.hpp>

#include <string>

using std::string;

#define INT(X)     std::get<int>(event.data[X])
#define FLOAT(X)   std::get<float>(event.data[X])
#define DOUBLE(X)  std::get<double>(event.data[X])
#define LONG(X)    std::get<long>(event.data[X])
#define STRING(X)  std::get<string>(event.data[X])
#define LAYER(X)   std::get<Layer*>(event.data[X])
#define SHADER(X)  std::get<Shader*>(event.data[X])
#define VOID(X)    std::get<void*>(event.data[X])
#define SURFACE(X) std::get<SDL_Surface*>(event.data[X])

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
    CameraUpdateRequest,
    AttributeModifyRequest,

    // Right now, ticks are just frames, but who's counting?
    Tick1,
    Tick10,
    Tick100,

    BeginDraw,
    EndDraw,

    MTBindTexture,
    MTNotifyMeshLoad,
    MTUpdateLayers,

    // Control events
    Break,
};

enum ResourceType {
    MeshResource,
    Texture,
};

struct resource_request_t {
    enum ResourceType type;
    string name;
};
