#pragma once

#include <plog/Log.h>
#include <plog/Appenders/ColorConsoleAppender.h>

#define VEC2(TARGET, INDEX, V1, V2) {\
    TARGET[2 * (INDEX)    ] = V1;\
    TARGET[2 * (INDEX) + 1] = V2;\
}

#define VEC3(TARGET, INDEX, V1, V2, V3) {\
    TARGET[3 * (INDEX)    ] = V1;\
    TARGET[3 * (INDEX) + 1] = V2;\
    TARGET[3 * (INDEX) + 2] = V3;\
}

#define VEC4(TARGET, INDEX, V1, V2, V3, V4) {\
    TARGET[4 * (INDEX)    ] = V1;\
    TARGET[4 * (INDEX) + 1] = V2;\
    TARGET[4 * (INDEX) + 2] = V3;\
    TARGET[4 * (INDEX) + 3] = V4;\
}
