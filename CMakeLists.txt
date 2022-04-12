cmake_minimum_required(VERSION 3.16)

add_compile_definitions(_ROOT_DIR="${CMAKE_SOURCE_DIR}")

project(NEWGL)
add_subdirectory(engine)
add_subdirectory(demo)

set_property(TARGET engine PROPERTY CXX_STANDARD 20)
set_property(TARGET demo PROPERTY CXX_STANDARD 20)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(freetype CONFIG REQUIRED)
find_package(sdl2-image CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

find_path(PLOG_INCLUDE_DIRS "plog/Appenders/AndroidAppender.h")

target_link_libraries(engine PUBLIC glfw)
target_link_libraries(engine PUBLIC glm::glm)
target_link_libraries(engine PUBLIC freetype)
target_link_libraries(engine PUBLIC SDL2::SDL2_image)
target_include_directories(engine PRIVATE ${PLOG_INCLUDE_DIRS})
target_link_libraries(engine PRIVATE imgui::imgui)
