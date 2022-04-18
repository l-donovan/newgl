#pragma once

#include "engine/common.h"

enum CustomEventType {
    EMPTY = static_cast<std::underlying_type_t<EventType>>(EventType::END),

    RecalculateView
};
