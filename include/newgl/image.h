#pragma once

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <SDL2/SDL.h>
#include <SDL_image.h>

struct format_t {
    GLint internal_format;
    GLint format;
    GLint type;
};

SDL_Surface* load_texture(const char* filename);
GLuint bind_texture(SDL_Surface *res_texture);
void debug_print_format(SDL_PixelFormat *format);
format_t gl_tex_params(SDL_PixelFormat *format);
