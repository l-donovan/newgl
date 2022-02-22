project(newgl)

include_directories(${NEWGL_SOURCE_DIR}/glad/include)
link_directories(${NEWGL_SOURCE_DIR}/glad/src)

include_directories(${NEWGL_SOURCE_DIR}/include)

add_executable(newgl
    engine.cpp
    main.cpp
    shader.cpp
    material.cpp
    window.cpp
    mesh.cpp
    image.cpp

    example_layer.cpp
    entity_layer.cpp
    sky_layer.cpp
    text_layer.cpp
    script_layer.cpp

    attributes/basic_motion.cpp
    attributes/capture_framerate.cpp
    attributes/first_person_camera.cpp
    attributes/toggle_wireframe.cpp)

target_link_libraries(newgl PRIVATE glad)
