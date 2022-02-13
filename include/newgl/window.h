#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <chrono>
#include <map>
#include <string>
#include <vector>

#include "global.h"
#include "engine.h"
#include "mesh.h"
#include "layer.h"
#include "mesh.h"
#include "shader.h"

using std::string;
using std::vector;

class Window {
    private:
        string window_title;
        int initial_width;
        int initial_height;
        GLFWwindow *win;
        vector<Shader*> shaders;

        std::map<std::string, mesh_t*> meshes;

        int fps_target = 60;
        std::chrono::duration<float, std::milli> single_frame_duration;
        std::chrono::duration<float, std::milli> current_frame_duration;

        void process_events();
        void draw();

        static Engine *engine;
        static std::map<EventType, std::vector<std::shared_ptr<Attribute>>> attribute_event_subscribers;

        static void global_cursor_pos_callback(GLFWwindow *window, double x_pos, double y_pos);
        static void global_window_size_callback(GLFWwindow *window, int width, int height);
        static void global_key_event_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
        static void send_event(Event e);

        void resize_window(int width, int height);
        mesh_t* load_mesh(string filename);
        void process_mesh_load_request(Layer *requesting_layer, string filename);
        void process_texture_load_request(Layer *requesting_layer, string filename);
    public:
        Window(string window_title, int initial_width, int initial_height);
        ~Window();

        bool startup();
        void main_loop();
        void add_layer(Layer *layer, Shader *shader);
        void attach(Engine *engine);

        static int width;
        static int height;
};
