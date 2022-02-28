#pragma once

// Forward declarations
class Attribute;
class Mesh;
class Shader;
class Layer;

#include "common.h"

#include <string>
#include <variant>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL_image.h>

#define EVENT_LAYER_ADD       0
#define EVENT_LAYER_ADD_BLANK 7

#define EVENT_CAMERA_POS_SET_PITCH 1
#define EVENT_CAMERA_POS_INC_PITCH 2
#define EVENT_CAMERA_POS_SET_YAW   3
#define EVENT_CAMERA_POS_INC_YAW   4

#define EVENT_ATTRIBUTE_ADD    5
#define EVENT_ATTRIBUTE_REMOVE 6

using variant_t = std::variant<
    int, float, double, long, bool, std::string,
    Layer*, Shader*, void*, SDL_Surface*, Mesh*>;

enum EventType {
    WindowResize,
    Key,
    CursorPosition,
    Scroll,
    Initialize,
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

    // This is necessary for custom events to work
    END,
};

struct Event {
    enum EventType type;
    std::vector<variant_t> data;
};
