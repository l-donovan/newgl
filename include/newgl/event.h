#pragma once

// Forward declarations
class Shader;
class Layer;

#include "common.h"

#include <string>
#include <variant>
#include <vector>

#define EVENT_LAYER_ADD 0

using variant_t = std::variant<int, float, double, long, Layer*, Shader*, std::string, void*>;

struct Event {
    enum EventType type;
    std::vector<variant_t> data;
};
