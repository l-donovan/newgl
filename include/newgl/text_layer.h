#pragma once
#include <glm/glm.hpp>

#include "layer.h"

#include <iostream>
#include <string>

using std::string;

struct Glyph {
    void         *data;       // Texture data
    glm::ivec2    size;       // Size of glyph
    glm::ivec2    bearing;    // Offset from baseline to left/top of glyph
    unsigned int  advance;    // Horizontal offset to advance to next glyph
    glm::vec2     uv_start;   // First UV coordinate pair
    glm::vec2     uv_stop;    // Second UV coordinate pair
};

class TextLayer : public Layer {
    private:
        // VBOs and IBOs
        GLuint vbo_vertices = 0;
        GLuint vbo_colors = 0;
        GLuint vbo_uvs = 0;
        GLuint ibo_faces = 0;

        // Atlas
        GLuint atlas_texture_id = 0;
        unsigned int atlas_width = 0;
        unsigned int atlas_height = 0;

        // Layer positioning
        float x = 0.0f;
        float y = 0.0f;

        float y_insert_pos_top = 1.0f;
        float y_insert_pos_bottom = 1.0f;
        int vertical_char_offset = 0;
        float scale = 0.5f;
        string text;

        unsigned int columns = 80;
        unsigned int rows = 24;
        unsigned int char_count = 80 * 24;
        int start_line = 0;
        int last_start_line = 0;
        float *vertices = nullptr;
        float *uvs = nullptr;
        float *colors = nullptr;
        GLushort *faces = nullptr;

        // Font
        std::string font_path;
        int font_height = 0;

        int replace_line_idx = 0;
    public:
        TextLayer();

        void set_font(string font_path, int font_height);
        bool rasterize_font();
        void setup();
        void update();
        void draw(glm::mat4 view, glm::mat4 projection, camera_t camera);
        void teardown();
        void set_text(string text);
        void set_position(float x, float y);
        void calculate_dimensions();
        void allocate_attribute_buffers();
        void calculate_attribute_buffers(bool full_draw = false);
        void receive_resource(ResourceType type, string name, void *data);

        void set_start_line(unsigned int line_num);
        unsigned int get_start_line();
};
