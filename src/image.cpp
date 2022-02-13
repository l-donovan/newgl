#include "newgl/image.h"

#include <SDL2/SDL.h>
#include <SDL_image.h>

#include <plog/Log.h>

GLuint load_texture(const char* filename) {
    GLuint texture_id;

    SDL_Surface *res_texture = IMG_Load(filename);
    if (res_texture == nullptr) {
        PLOGE << "Error: can't load image " << filename << ": " << SDL_GetError();
        return -1; // TODO
    }

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8, // TODO hardcoded
        res_texture->w,
        res_texture->h,
        0,
        GL_BGRA, // TODO hardcoded
        GL_UNSIGNED_INT_8_8_8_8_REV,
        res_texture->pixels);
    SDL_FreeSurface(res_texture);

    PLOGD << "Loaded texture " << filename;

    return texture_id;
}
