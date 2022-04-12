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
#define EVENT_LAYER_ADD_BLANK 1

// TODO: Use something quicker than variant
using variant_t = std::variant<
    int, float, double, long, bool,
    std::string, // TODO: Replace this with a simpler char pointer
    Layer*, Shader*, void*, SDL_Surface*, Mesh*>;

GENERATE_ENUM_WITH_NAMES(EventType,
    WindowResize,
    Key,
    CursorPosition,
    Scroll,
    Initialize,
    MeshLoad,
    TextureLoad,
    Framerate,
    MouseButton,

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
    END
)

struct Event {
    enum EventType type;
    std::vector<variant_t> data;
};
