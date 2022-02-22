#include "newgl/image.h"

#include <SDL2/SDL.h>
#include <SDL_image.h>

#include <plog/Log.h>

#include <string>

using std::string;

void print_format(SDL_PixelFormat *format) {
    char fmt_str[format->BitsPerPixel + 1];
    fmt_str[format->BitsPerPixel] = 0;
    unsigned int mask;

    for (unsigned int i = 0; i < format->BitsPerPixel; ++i) {
        mask = 1 << i;

        if ((format->Rmask & mask) == mask)
            fmt_str[format->BitsPerPixel - i - 1] = 'R';
        else if ((format->Gmask & mask) == mask)
            fmt_str[format->BitsPerPixel - i - 1] = 'G';
        else if ((format->Bmask & mask) == mask)
            fmt_str[format->BitsPerPixel - i - 1] = 'B';
        else if ((format->Amask & mask) == mask)
            fmt_str[format->BitsPerPixel - i - 1] = 'A';
        else
            fmt_str[format->BitsPerPixel - i - 1] = '-';
    }

    PLOGD << "Format: (MSB) " << string(fmt_str) << " (LSB)";
}

SDL_Surface* load_texture(const char* filename) {
    SDL_Surface *res_texture = IMG_Load(filename);

    if (res_texture == nullptr) {
        PLOGE << "Error: can't load image " << filename << ": " << SDL_GetError();
    } else {
        PLOGD << "Loaded texture " << filename;
    }

    return res_texture;
}

GLuint bind_texture(SDL_Surface *res_texture) {
    print_format(res_texture->format);

    GLuint texture_id;

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // I have no idea why pixel data comes in as BGR(A) instead of
    // as RGB(A) on macOS, but it does
    glTexImage2D(
        GL_TEXTURE_2D,               // Target
        0,                           // Level
        GL_RGBA8,                    // Internal format
        res_texture->w,              // Width
        res_texture->h,              // Height
        0,                           // Border
#if __APPLE__
        GL_BGRA,                     // Format
        GL_UNSIGNED_INT_8_8_8_8_REV, // Type
#else
        GL_RGBA,                     // Format
        GL_UNSIGNED_BYTE,            // Type
#endif
        res_texture->pixels);        // Data

    SDL_FreeSurface(res_texture);

    return texture_id;
}
