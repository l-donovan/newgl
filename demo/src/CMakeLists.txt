include_directories(${NEWGL_SOURCE_DIR}/engine/glad/include)
link_directories(${NEWGL_SOURCE_DIR}/engine/glad/src)

include_directories(${NEWGL_SOURCE_DIR}/engine/include)
link_directories(${NEWGL_SOURCE_DIR}/engine/src)

include_directories(${NEWGL_SOURCE_DIR}/demo/include)

add_executable(demo 
    attributes/basic_motion.cpp
    attributes/capture_framerate.cpp
    attributes/first_person_camera.cpp
    attributes/movement_controller.cpp
    attributes/toggle_wireframe.cpp

    demo_controller.cpp
    main.cpp)

target_link_libraries(demo engine)