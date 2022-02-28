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

#include "engine/global.h"
#include "engine/application.h"
#include "engine/common.h"
#include "engine/controller.h"
#include "engine/event.h"
#include "engine/image.h"
#include "engine/layer.h"
#include "engine/material.h"
#include "engine/mesh.h"
#include "engine/shader.h"

using std::string;

Controller* Application::controller = nullptr;
Application::AttributeEventMap Application::attribute_event_subscribers = {};
camera_t Application::cameras[256];

int Application::width = 0;
int Application::height = 0;

int frame_count = 0;
float frame_duration_sum = 0.0f;
int fps_samples = 60;
long total_frame_count = 0;

Application::Application(string window_title, int initial_width, int initial_height) {
    this->window_title = window_title;
    this->initial_width = initial_width;
    this->initial_height = initial_height;

    this->single_frame_duration = std::chrono::duration<double, std::milli>(1000.0f / this->fps_target);
}

Application::~Application() {}

void Application::attach(Controller *controller) {
    Application::controller = controller;
}

void Application::global_cursor_pos_callback(GLFWwindow *window, double x_pos, double y_pos) {
    double x_adj = x_pos / Application::width;
    double y_adj = y_pos / Application::height;

    Application::send_event({EventType::CursorPosition, {x_adj, y_adj}});
}

void Application::global_window_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);

    Application::width = width;
    Application::height = height;

    Application::send_event({EventType::WindowResize, {width, height}});
}

void Application::global_key_event_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    } else {
        Application::send_event({EventType::Key, {key, scancode, action, mods}});
    }
}

void Application::global_scroll_event_callback(GLFWwindow *window, double x_offset, double y_offset) {
    Application::send_event({EventType::Scroll, {x_offset, y_offset}});
}

void Application::send_event(Event e) {
    Application::controller->incoming_events.enqueue(e);

    auto attributes = Application::attribute_event_subscribers[e.type];

    for (auto attr = attributes.begin(); attr != attributes.end(); ++attr) {
        (*attr)->receive_event(e);
    };
}

static void glfw_error_callback(int error, const char *description) {
    PLOGE << "Error: " << description;
}

bool Application::startup() {
    // First we want to check if the attached Controller needs us to do anything.
    // Any GL/GLFW-specific events will absolutely break things, however, since
    // neither are initialized at this point.

    this->process_events(true);

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

    glfwSetKeyCallback(this->win, Application::global_key_event_callback);
    glfwSetCursorPosCallback(this->win, Application::global_cursor_pos_callback);
    glfwSetWindowSizeCallback(this->win, Application::global_window_size_callback);
    glfwSetScrollCallback(this->win, Application::global_scroll_event_callback);

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
    glfwGetWindowSize(this->win, &Application::width, &Application::height);

    // Disable the cursor
    glfwSetInputMode(this->win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return true;
}

void Application::draw() {
    // Clear the color and depth buffers
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camera_t *camera = Application::get_camera(0);

    // TODO: We should have a designated skybox to draw separately

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

                Application::send_event({EventType::BeginDraw, {layer}});
                layer->draw(camera->view, camera->projection, *camera);
                Application::send_event({EventType::EndDraw, {layer}});
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

                Application::send_event({EventType::BeginDraw, {layer}});
                if (layer->material != nullptr) {
                    layer->material->apply(shader);
                }
                layer->draw(camera->view, camera->projection, *camera);
                Application::send_event({EventType::EndDraw, {layer}});
            }
        }
    }

    glfwSwapBuffers(this->win);
}

void Application::resize_window(int width, int height) {
    PLOGI << "Got window resize request";
    PLOGI << "W: " << width << ", H: " << height;

    glfwSetWindowSize(this->win, width, height);
    glViewport(0, 0, width, height);
}

Mesh* Application::load_mesh(string filename) {
    auto it = this->meshes.find(filename);

    if (it != this->meshes.end()) {
        PLOGD << "Loaded existing mesh " << filename;
        return &it->second;
    }

    Mesh mesh = Mesh::from_obj(filename);
    this->meshes[filename] = mesh;

    return &this->meshes[filename];
}

void Application::process_mesh_load_request(Layer *requesting_layer, string filename) {
    Mesh *mesh = this->load_mesh(filename);

    Application::mt_queue.enqueue({EventType::MTNotifyMeshLoad, {
        requesting_layer,
        filename,
        mesh
    }});
}

void Application::process_texture_load_request(Layer *requesting_layer, string filename) {
    auto it = this->textures.find(filename);

    if (it != this->textures.end()) {
        PLOGD << "Loaded existing texture " << filename;

        void *ptr = malloc(sizeof(GLuint));

        if (ptr == nullptr) {
            PLOGE << "Failed to allocate GLuint pointer. Can't bind texture!";
            return;
        }

        *((int*)ptr) = it->second;

        Application::controller->incoming_events.enqueue({EventType::TextureLoad, {
            requesting_layer,
            filename,
            ptr,
        }});
    } else {
        SDL_Surface *texture = load_texture(filename.c_str());

        Application::mt_queue.enqueue({EventType::MTBindTexture, {
            requesting_layer,
            filename,
            texture,
        }});
    }
}

void Application::process_mt_bind_texture(Layer *requesting_layer, string filename, SDL_Surface *texture) {
    GLuint texture_id = bind_texture(texture);

    void *ptr = malloc(sizeof(GLuint));

    if (ptr == nullptr) {
        PLOGE << "Failed to allocate GLuint pointer. Can't bind texture!";
        return;
    }

    *((int*)ptr) = texture_id;

    this->textures[filename] = texture_id;

    Application::controller->incoming_events.enqueue({EventType::TextureLoad, {
        requesting_layer,
        filename,
        ptr,
    }});
}

void Application::process_events(bool single_pass) {
    // This is where the application acts on the events sent from the controller
    Event event;

    while (!single_pass || !Application::controller->outgoing_events.empty()) {
        event = Application::controller->outgoing_events.dequeue();

        switch (event.type) {
        case EventType::WindowResizeRequest:
            this->resize_window(INT(0), INT(1));
            break;
        case EventType::LayerUpdateRequest:
            this->mt_queue.enqueue({EventType::MTUpdateLayers, {}});
            break;
        case EventType::LayerModifyRequest:
            if (INT(0) == EVENT_LAYER_ADD) {
                PLOGD << "Got layer add request";
                this->add_layer(LAYER(1), SHADER(2));
            } else if (INT(0) == EVENT_LAYER_ADD_BLANK) {
                PLOGD << "Got blank layer add request";
                this->add_layer(LAYER(1), nullptr);
            }
            break;
        case EventType::MeshLoadRequest:
            PLOGD << "Got mesh load request";
            this->process_mesh_load_request(LAYER(0), STRING(1));
            break;
        case EventType::TextureLoadRequest:
            PLOGD << "Got texture load request";
            this->process_texture_load_request(LAYER(0), STRING(1));
            break;
        case EventType::CameraUpdateRequest:
            break;
        case EventType::Break:
            PLOGI << "Got Break event. Exiting";
            return;
        default:
            PLOGW << "Got unknown event type";
            break;
        }
    }
}

void Application::process_mt_events() {
    Event event;

    // This is run from the main thread (MT), thus it can't block while waiting for more
    // events to be added to the queue
    while (!this->mt_queue.empty()) {
        event = this->mt_queue.dequeue();

        switch (event.type) {
        case EventType::MTBindTexture:
            this->process_mt_bind_texture(LAYER(0), STRING(1), SURFACE(2));
            break;
        case EventType::MTNotifyMeshLoad:
            LAYER(0)->receive_resource(MeshResource, STRING(1), (void*) MESH(2));
            break;
        case EventType::MTUpdateLayers:
            for (Shader *shader : this->shaders) {
                shader->update();
            }
            break;
        default:
            break;
        }
    }
}

void Application::main_loop() {
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    std::thread application_event_thread(&Application::process_events, this, false);
    std::thread controller_event_thread(&Controller::process_events, Application::controller, false);

    auto start = std::chrono::high_resolution_clock::now();
    auto stop = std::chrono::high_resolution_clock::now();

    std::chrono::duration<float, std::milli> sleep_duration;
    bool ahead;

    Application::send_event({EventType::WindowResize, {Application::width, Application::height}});

    while (!glfwWindowShouldClose(this->win)) {
        // Start the timer where we left off
        start = stop;

        Application::controller->process_resource_requests();

        this->process_mt_events();

        // Draw frame and time the draw call
        this->draw();

        // Poll for any new glfw events
        glfwPollEvents();

        // Send tick events
        Application::send_event({EventType::Tick1, {total_frame_count}});

        if (total_frame_count % 10 == 0)
            Application::send_event({EventType::Tick10, {total_frame_count}});

        if (total_frame_count % 100 == 0)
            Application::send_event({EventType::Tick100, {total_frame_count}});

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
                Application::send_event({EventType::Framerate, {float(this->fps_target), ahead_percent}});
            } else {
                // We're behind schedule!
                auto fps = 1000.0f / (frame_duration_sum / fps_samples);
                float behind_percent = -100.0f * sleep_duration.count() / (1000.0f / this->fps_target);
                PLOGW << "FPS: " << fps << ", behind " << behind_percent << "%";
                Application::send_event({EventType::Framerate, {fps, -behind_percent}});
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

    Application::controller->outgoing_events.enqueue({EventType::Break, {}});
    Application::controller->incoming_events.enqueue({EventType::Break, {}});

    application_event_thread.join();
    controller_event_thread.join();

    for (Shader *shader : this->shaders) {
        // Begin tearing down GL resources
        shader->teardown();
    }

    glDeleteVertexArrays(1, &vao);
    glfwDestroyWindow(this->win);
    glfwTerminate();
}

void Application::add_layer(Layer *layer, Shader *shader) {
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
    }
}

void Application::add_attribute(std::shared_ptr<Attribute> attr) {
    std::optional<EventType> event_type;

    while ((event_type = attr->pop_subscription_request()).has_value()) {
        if (*event_type == EventType::Initialize) {
            attr->receive_event({EventType::Initialize, {}});
        } else {
            Application::attribute_event_subscribers[*event_type].push_back(attr);
        }
    }
}

camera_t* Application::get_camera(uint8_t idx) {
    return &Application::cameras[idx];
}
