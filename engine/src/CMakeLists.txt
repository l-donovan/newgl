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
    layers/script_layer.cpp

    attributes/basic_motion.cpp
    attributes/capture_framerate.cpp
    attributes/first_person_camera.cpp
    attributes/toggle_wireframe.cpp)

target_link_libraries(engine PRIVATE glad)
