include_directories(${NEWGL_SOURCE_DIR}/engine/glad/include)
link_directories(${NEWGL_SOURCE_DIR}/engine/glad/src)

include_directories(${NEWGL_SOURCE_DIR}/engine/include)
link_directories(${NEWGL_SOURCE_DIR}/engine/src)

include_directories(${NEWGL_SOURCE_DIR}/demo/include)

add_executable(demo 
    demo_controller.cpp
    main.cpp)

target_link_libraries(demo engine)