#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <plog/Log.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "newgl/global.h"
#include "newgl/common.h"
#include "newgl/engine.h"
#include "newgl/event.h"
#include "newgl/image.h"
#include "newgl/layer.h"
#include "newgl/material.h"
#include "newgl/mesh.h"
#include "newgl/shader.h"
#include "newgl/window.h"

using std::string;

Engine* Window::engine = nullptr;
Window::AttributeEventMap Window::attribute_event_subscribers = {};

int Window::width = 0;
int Window::height = 0;

glm::mat4 Window::view = glm::mat4(1.0);

int frame_count = 0;
float frame_duration_sum = 0.0f;
int fps_samples = 60;
long total_frame_count = 0;

Window::Window(string window_title, int initial_width, int initial_height) {
    this->window_title = window_title;
    this->initial_width = initial_width;
    this->initial_height = initial_height;

    this->single_frame_duration = std::chrono::duration<double, std::milli>(1000.0f / this->fps_target);
}

Window::~Window() {}

void Window::attach(Engine *engine) {
    Window::engine = engine;
}

void Window::global_cursor_pos_callback(GLFWwindow *window, double x_pos, double y_pos) {
    double x_adj = x_pos / Window::width;
    double y_adj = y_pos / Window::height;

    if (x_adj < 0.0 || x_adj > 1.0 || y_adj < 0.0 || y_adj > 1.0)
        return;

    Window::send_event({CursorPosition, {x_adj, y_adj}});
}

void Window::global_window_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);

    Window::width = width;
    Window::height = height;

    Window::send_event({WindowResize, {width, height}});
}

void Window::global_key_event_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    } else {
        Window::send_event({Key, {key, scancode, action, mods}});
    }
}

void Window::send_event(Event e) {
    Window::engine->incoming_events.enqueue(e);

    auto attributes = Window::attribute_event_subscribers[e.type];

    for (auto attr = attributes.begin(); attr != attributes.end(); ++attr) {
        (*attr)->receive_event(e);
    };
}

static void glfw_error_callback(int error, const char *description) {
    PLOGE << "Error: " << description;
}

bool Window::startup() {
    // First we want to check if the attached Engine needs us to do anything.
    // Any GL/GLFW-specific events will absolutely break things, however, since
    // neither are initialized at this point.

    Window::process_events(this, true);

    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit()) {
        PLOGE << "Error: glfwInit";
        return false;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    this->win = glfwCreateWindow(
            this->initial_width, this->initial_height,
            this->window_title.c_str(),
            nullptr, nullptr);

    if (!this->win) {
        glfwTerminate();
        PLOGE << "Error: can't create window";
        return false;
    }

    glfwSetKeyCallback(this->win, Window::global_key_event_callback);
    glfwSetCursorPosCallback(this->win, Window::global_cursor_pos_callback);
    glfwSetWindowSizeCallback(this->win, Window::global_window_size_callback);

    glfwMakeContextCurrent(this->win);
    gladLoadGL();
    glfwSwapInterval(1);

    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Shader compilation is deferred
    for (Shader *shader : this->shaders) {
        shader->setup();
    }

    // Set our initial window size
    glfwGetWindowSize(this->win, &Window::width, &Window::height);

    return true;
}

glm::mat4 construct_view(glm::vec3 eye, float pitch, float yaw) {
    float cos_pitch = cos(pitch);
    float sin_pitch = sin(pitch);
    float cos_yaw = cos(yaw);
    float sin_yaw = sin(yaw);

    glm::vec3 x_axis(cos_yaw, 0, -sin_yaw);
    glm::vec3 y_axis(sin_yaw * sin_pitch, cos_pitch, cos_yaw * sin_pitch);
    glm::vec3 z_axis(sin_yaw * cos_pitch, -sin_pitch, cos_pitch * cos_yaw);

    // Create a 4x4 view matrix from the right, up, forward and eye position vectors
    return {
        {x_axis.x, y_axis.x, z_axis.x, 0},
        {x_axis.y, y_axis.y, z_axis.y, 0},
        {x_axis.z, y_axis.z, z_axis.z, 0},
        {-glm::dot(x_axis, eye), -glm::dot(y_axis, eye), -glm::dot(z_axis, eye), 1}
    };
}

void Window::draw() {
    // Clear the colorbuffer
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    struct camera_t camera = {
        .position = glm::vec3(0.0, 2.0, 4.0),
        .target   = glm::vec3(0.0, 0.0, 0.0),
        .up       = glm::vec3(0.0, 1.0, 0.0),
        .vfov     = 45.0
    };

    float ratio = 1.0f * Window::width / Window::height;
    //glm::mat4 view = glm::lookAt(camera.position, camera.target, camera.up);
    glm::mat4 projection = glm::perspective(camera.vfov, ratio, 0.1f, 100.0f);

    // TODO: We should sort the draws by depth, or at least set aside a skybox to draw separately

    glDepthMask(GL_FALSE);

    bool used;

    for (Shader *shader : this->shaders) {
        used = false;

        for (Layer *layer : shader->layers) {
            if (!layer->depth_enabled()) {
                if (!used) {
                    shader->use();
                    used = true;
                }

                Window::send_event({BeginDraw, {layer}});
                layer->draw(Window::view, projection, camera);
                Window::send_event({EndDraw, {layer}});
            }
        }
    }

    glDepthMask(GL_TRUE);

    for (Shader *shader : this->shaders) {
        used = false;

        for (Layer *layer : shader->layers) {
            if (layer->depth_enabled()) {
                if (!used) {
                    shader->use();
                    used = true;
                }

                Window::send_event({BeginDraw, {layer}});
                if (layer->material != nullptr) {
                    layer->material->apply(shader);
                }
                layer->draw(Window::view, projection, camera);
                Window::send_event({EndDraw, {layer}});
            }
        }
    }

    glfwSwapBuffers(this->win);
}

void Window::resize_window(int width, int height) {
    PLOGI << "Got window resize request";
    PLOGI << "W: " << width << ", H: " << height;

    PLOGI << "RESIZE - " << std::this_thread::get_id();

    glfwSetWindowSize(this->win, width, height);
    glViewport(0, 0, width, height);
}

Mesh* Window::load_mesh(string filename) {
    auto it = this->meshes.find(filename);

    if (it != this->meshes.end()) {
        PLOGD << "Loaded existing mesh " << filename;
        return &it->second;
    }

    Mesh mesh = Mesh::from_obj(filename);
    this->meshes[filename] = mesh;

    return &this->meshes[filename];
}

void Window::process_mesh_load_request(Layer *requesting_layer, string filename) {
    Mesh *mesh = this->load_mesh(filename);

    Window::mt_queue.enqueue({MTNotifyMeshLoad, {
        requesting_layer,
        filename,
        mesh
    }});
}

void Window::process_texture_load_request(Layer *requesting_layer, string filename) {
    auto it = this->textures.find(filename);

    if (it != this->textures.end()) {
        PLOGD << "Loaded existing texture " << filename;

        void *ptr = malloc(sizeof(GLuint));

        if (ptr == nullptr) {
            PLOGE << "Failed to allocate GLuint pointer. Can't bind texture!";
            return;
        }

        *((int*)ptr) = it->second;

        Window::engine->incoming_events.enqueue({TextureLoad, {
            requesting_layer,
            filename,
            ptr,
        }});
    } else {
        SDL_Surface *texture = load_texture(filename.c_str());

        Window::mt_queue.enqueue({MTBindTexture, {
            requesting_layer,
            filename,
            texture,
        }});
    }
}

void Window::process_mt_bind_texture(Layer *requesting_layer, string filename, SDL_Surface *texture) {
    GLuint texture_id = bind_texture(texture);

    void *ptr = malloc(sizeof(GLuint));

    if (ptr == nullptr) {
        PLOGE << "Failed to allocate GLuint pointer. Can't bind texture!";
        return;
    }

    *((int*)ptr) = texture_id;

    this->textures[filename] = texture_id;

    Window::engine->incoming_events.enqueue({TextureLoad, {
        requesting_layer,
        filename,
        ptr,
    }});
}

void Window::process_events(Window *win, bool single_pass) {
    // This is where the window acts on the events sent from the engine
    Event event;

    while (!single_pass || !Window::engine->outgoing_events.empty()) {
        event = Window::engine->outgoing_events.dequeue();

        switch (event.type) {
        case WindowResizeRequest:
            win->resize_window(INT(0), INT(1));
            break;
        case LayerUpdateRequest:
            win->mt_queue.enqueue({MTUpdateLayers, {}});
            break;
        case LayerModifyRequest:
            if (INT(0) == EVENT_LAYER_ADD) {
                PLOGD << "Got layer add request";
                win->add_layer(LAYER(1), SHADER(2));
            } else if (INT(0) == EVENT_LAYER_ADD_BLANK) {
                PLOGD << "Got blank layer add request";
                win->add_layer(LAYER(1), nullptr);
            }
            break;
        case MeshLoadRequest:
            PLOGD << "Got mesh load request";
            win->process_mesh_load_request(LAYER(0), STRING(1));
            break;
        case TextureLoadRequest:
            PLOGD << "Got texture load request";
            win->process_texture_load_request(LAYER(0), STRING(1));
            break;
        case CameraUpdateRequest:
            break;
        case Break:
            PLOGI << "Got Break event. Exiting";
            return;
        default:
            PLOGW << "Got unknown event type";
            break;
        }
    }
}

void Window::process_mt_events() {
    Event event;

    // This is run from the main thread (MT), thus it can't block while waiting for more
    // events to be added to the queue
    while (!this->mt_queue.empty()) {
        event = this->mt_queue.dequeue();

        switch (event.type) {
        case MTBindTexture:
            this->process_mt_bind_texture(LAYER(0), STRING(1), SURFACE(2));
            break;
        case MTNotifyMeshLoad:
            LAYER(0)->receive_resource(MeshResource, STRING(1), (void*) MESH(2));
            break;
        case MTUpdateLayers:
            for (Shader *shader : this->shaders) {
                shader->update();
            }
            break;
        default:
            break;
        }
    }
}

void Window::main_loop() {
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    std::thread window_event_thread(Window::process_events, this, false);
    std::thread engine_event_thread(Engine::process_events, Window::engine, false);

    auto start = std::chrono::high_resolution_clock::now();
    auto stop = std::chrono::high_resolution_clock::now();

    std::chrono::duration<float, std::milli> sleep_duration;
    bool ahead;

    while (!glfwWindowShouldClose(this->win)) {
        // Start the timer where we left off
        start = stop;

        Window::engine->process_resource_requests();

        this->process_mt_events();

        // Draw frame and time the draw call
        this->draw();

        // Poll for any new glfw events
        glfwPollEvents();

        // Send tick events
        Window::send_event({Tick1, {total_frame_count}});

        if (total_frame_count % 10 == 0)
            Window::send_event({Tick10, {total_frame_count}});

        if (total_frame_count % 100 == 0)
            Window::send_event({Tick100, {total_frame_count}});

        // Calculate the time it took to draw the current frame, and determine if we are ahead
        // of or behind schedule
        stop = std::chrono::high_resolution_clock::now();
        this->current_frame_duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        sleep_duration = this->single_frame_duration - this->current_frame_duration;
        ahead = sleep_duration.count() > 0;

        if (frame_count++ < fps_samples) {
            frame_duration_sum += this->current_frame_duration.count();
        } else {
            if (ahead) {
                // We're ahead of schedule! (Or at least on time)
                float ahead_percent = 100.0f * sleep_duration.count() / (1000.0f / this->fps_target);
                PLOGD << "FPS: " << this->fps_target << ", ahead " << ahead_percent << "%";
                Window::send_event({Framerate, {float(this->fps_target), ahead_percent}});
            } else {
                // We're behind schedule!
                auto fps = 1000.0f / (frame_duration_sum / fps_samples);
                float behind_percent = -100.0f * sleep_duration.count() / (1000.0f / this->fps_target);
                PLOGW << "FPS: " << fps << ", behind " << behind_percent << "%";
                Window::send_event({Framerate, {fps, -behind_percent}});
            }

            frame_count = 0;
            frame_duration_sum = 0.0f;
        }

        // Only sleep if we're ahead of schedule
        if (ahead) {
            std::this_thread::sleep_for(sleep_duration);
        }

        // Reset the clock after any sleeping overhead or actual sleeping
        stop = std::chrono::high_resolution_clock::now();

        ++total_frame_count;
    }

    Window::engine->outgoing_events.enqueue({Break, {}});
    Window::engine->incoming_events.enqueue({Break, {}});

    window_event_thread.join();
    engine_event_thread.join();

    for (Shader *shader : this->shaders) {
        // Begin tearing down GL resources
        shader->teardown();
    }

    glDeleteVertexArrays(1, &vao);
    glfwDestroyWindow(this->win);
    glfwTerminate();
}

void Window::add_layer(Layer *layer, Shader *shader) {
    // Layers are actually "stored" in the shader.
    // this makes rendering by shader easier, and
    // minimizes program switching.

    if (shader != nullptr) {
        shader->add_layer(layer);

        // TODO: Can probably make this into an unordered_set or something faster
        if (std::find(this->shaders.begin(), this->shaders.end(), shader) == this->shaders.end()) {
            this->shaders.push_back(shader);
        }
    }

    auto attributes = layer->get_attributes();

    for (auto attr = attributes.begin(); attr != attributes.end(); ++attr) {
        this->add_attribute(*attr);
        // std::optional<EventType> event_type;
        // while ((event_type = (*attr)->pop_subscription_request()).has_value()) {
        //     Window::attribute_event_subscribers[*event_type].push_back(*attr);
        // }
    }
}

void Window::add_attribute(std::shared_ptr<Attribute> attr) {
    std::optional<EventType> event_type;

    while ((event_type = attr->pop_subscription_request()).has_value()) {
        Window::attribute_event_subscribers[*event_type].push_back(attr);
    }
}
