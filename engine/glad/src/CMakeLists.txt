project(glad)

include_directories(${NEWGL_SOURCE_DIR}/engine/glad/include)
add_library(glad glad.c)
