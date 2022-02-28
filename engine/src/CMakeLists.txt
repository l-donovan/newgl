project(engine)

include_directories(${NEWGL_SOURCE_DIR}/engine/glad/include)
link_directories(${NEWGL_SOURCE_DIR}/engine/glad/src)

include_directories(${NEWGL_SOURCE_DIR}/engine/include)

add_library(engine
    shader.cpp
    application.cpp
    material.cpp
    mesh.cpp
    image.cpp

    layers/example_layer.cpp
    layers/entity_layer.cpp
    layers/sky_layer.cpp
    layers/text_layer.cpp
    layers/script_layer.cpp)

target_link_libraries(engine PRIVATE glad)
