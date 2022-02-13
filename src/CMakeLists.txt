project(newgl)

include_directories(${NEWGL_SOURCE_DIR}/glad/include)
link_directories(${NEWGL_SOURCE_DIR}/glad/src)

include_directories(${NEWGL_SOURCE_DIR}/include)

add_executable(newgl
    engine.cpp
    main.cpp
    shader.cpp
    example_layer.cpp
    entity_layer.cpp
    text_layer.cpp
    window.cpp
    mesh.cpp
    image.cpp

    attributes/test.cpp
    attributes/capture_framerate.cpp)

target_link_libraries(newgl PRIVATE glad)
