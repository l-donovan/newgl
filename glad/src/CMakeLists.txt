project(glad)

include_directories(${NEWGL_SOURCE_DIR}/glad/include)
add_library(glad glad.c)
