#pragma once

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <SDL2/SDL.h>
#include <SDL_image.h>

SDL_Surface* load_texture(const char* filename);
GLuint bind_texture(SDL_Surface *res_texture);
