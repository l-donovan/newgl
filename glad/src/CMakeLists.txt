project(glad)

include_directories(${VIGOR_SOURCE_DIR}/glad/include)
add_library(glad glad.c)
