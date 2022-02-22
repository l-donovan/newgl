#include "newgl/image.h"

#include <SDL2/SDL.h>
#include <SDL_image.h>

#include <plog/Log.h>

#include <string>

using std::string;

void debug_print_format(SDL_PixelFormat *format) {
    Uint8 bpp = format->BitsPerPixel;

    char *fmt_str = (char*) malloc(sizeof(char) * (bpp + 1));
    fmt_str[bpp] = 0;

    unsigned int mask;

    for (unsigned int i = 0; i < bpp; ++i) {
        mask = 1 << i;

        if ((format->Rmask & mask) == mask)
            fmt_str[bpp - i - 1] = 'R';
        else if ((format->Gmask & mask) == mask)
            fmt_str[bpp - i - 1] = 'G';
        else if ((format->Bmask & mask) == mask)
            fmt_str[bpp - i - 1] = 'B';
        else if ((format->Amask & mask) == mask)
            fmt_str[bpp - i - 1] = 'A';
        else
            fmt_str[bpp - i - 1] = '-';
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

format_t gl_tex_params(SDL_PixelFormat *pixel_format) {
    GLint internal_format;
    GLint format;
    GLint type;

    switch (pixel_format->format) {
    case SDL_PIXELFORMAT_RGB888:
        internal_format = GL_RGBA8;
        format = GL_BGRA;
        type = GL_UNSIGNED_INT_8_8_8_8_REV;
        break;
    case SDL_PIXELFORMAT_RGB24:
        internal_format = GL_RGB8;
        format = GL_RGB;
        type = GL_UNSIGNED_BYTE;
    default:
        internal_format = GL_RGB;
        format = GL_RGB;
        type = GL_UNSIGNED_BYTE;
        break;
    }

    return {
        internal_format,
        format,
        type
    };
}

GLuint bind_texture(SDL_Surface *res_texture) {
    debug_print_format(res_texture->format);
    struct format_t params = gl_tex_params(res_texture->format);

    GLuint texture_id;

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // I have no idea why pixel data comes in as BGR(A) instead of
    // as RGB(A) on macOS, but it does
    glTexImage2D(
        GL_TEXTURE_2D,          // Target
        0,                      // Level
        params.internal_format, // Internal format
        res_texture->w,         // Width
        res_texture->h,         // Height
        0,                      // Border
        params.format,          // Format
        params.type,            // Type
        res_texture->pixels);   // Data

    SDL_FreeSurface(res_texture);

    return texture_id;
}
