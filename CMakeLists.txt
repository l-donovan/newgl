cmake_minimum_required(VERSION 3.16)

add_compile_definitions(_ROOT_DIR="${CMAKE_SOURCE_DIR}")

project(NEWGL)
add_subdirectory(glad)
add_subdirectory(src)

set_property(TARGET newgl PROPERTY CXX_STANDARD 20)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(freetype CONFIG REQUIRED)
find_package(sdl2-image CONFIG REQUIRED)

find_path(PLOG_INCLUDE_DIRS "plog/Appenders/AndroidAppender.h")

target_link_libraries(newgl PUBLIC glfw)
target_link_libraries(newgl PUBLIC freetype)
target_link_libraries(newgl PUBLIC SDL2::SDL2_image)
target_include_directories(newgl PRIVATE ${PLOG_INCLUDE_DIRS})
