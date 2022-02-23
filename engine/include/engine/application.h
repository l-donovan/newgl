#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <chrono>
#include <map>
#include <string>
#include <vector>

#include "global.h"
#include "controller.h"
#include "mesh.h"
#include "layer.h"
#include "safe_queue.h"
#include "shader.h"

using std::string;
using std::vector;

class Application {
    private:
        string window_title;
        int initial_width;
        int initial_height;
        GLFWwindow *win;
        vector<Shader*> shaders;

        std::map<std::string, Mesh> meshes;
        std::map<std::string, GLuint> textures;

        int fps_target = 60;
        std::chrono::duration<float, std::milli> single_frame_duration;
        std::chrono::duration<float, std::milli> current_frame_duration;

        void draw();

        static Controller *controller;

        typedef std::map<EventType, std::vector<std::shared_ptr<Attribute>>> AttributeEventMap;
        static AttributeEventMap attribute_event_subscribers;

        static void global_cursor_pos_callback(GLFWwindow *window, double x_pos, double y_pos);
        static void global_window_size_callback(GLFWwindow *window, int width, int height);
        static void global_key_event_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
        static void send_event(Event e);

        void resize_window(int width, int height);
        Mesh* load_mesh(string filename);
        void process_mesh_load_request(Layer *requesting_layer, string filename);
        void process_texture_load_request(Layer *requesting_layer, string filename);

        void process_events(bool single_pass);
        void process_mt_events();
        void process_mt_bind_texture(Layer *requesting_layer, string filename, SDL_Surface *texture);

        static camera_t cameras[256];
    public:
        Application(string window_title, int initial_width, int initial_height);
        ~Application();

        bool startup();
        void main_loop();
        void add_layer(Layer *layer, Shader *shader);
        void add_attribute(std::shared_ptr<Attribute> attr);
        void attach(Controller *controller);

        static int width;
        static int height;

        static glm::mat4 view; // TODO: Do this a better way
        static struct camera_t camera;

        SafeQueue<Event> mt_queue;

        static camera_t* get_camera(uint8_t idx);
};
