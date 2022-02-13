#include "newgl/global.h"
#include "newgl/common.h"
#include "newgl/engine.h"
#include "newgl/event.h"
#include "newgl/shader.h"
#include "newgl/example_layer.h"
#include "newgl/entity_layer.h"
#include "newgl/text_layer.h"
#include "newgl/window.h"

#include <optional>
#include <string>

// `_ROOT_DIR` is set via cmake
std::string ROOT_DIR(_ROOT_DIR);

Shader base_shader(
    ROOT_DIR + "/resources/shaders/base.v.glsl",
    ROOT_DIR + "/resources/shaders/base.f.glsl");

Shader text_shader(
    ROOT_DIR + "/resources/shaders/text.v.glsl",
    ROOT_DIR + "/resources/shaders/text.f.glsl");

EntityLayer entity_layer;
TextLayer framerate_layer;

Engine::Engine() {
}

Engine::~Engine() {
}

void Engine::pre_window_startup() {
    // We have to send some events to the window to setup our layers and shaders
    this->add_outgoing_event({LayerModifyRequest, {
        EVENT_LAYER_ADD,
        &entity_layer,
        &base_shader
    }});

    this->add_outgoing_event({LayerModifyRequest, {
        EVENT_LAYER_ADD,
        &framerate_layer,
        &text_shader
    }});
}

// This must be called after the window has had its `startup` called
void Engine::post_window_startup() {
#ifdef _WIN32
    framerate_layer.set_font("C:\\Windows\\Fonts\\IBMPlexMono-Regular.ttf", 14);
#elif __APPLE__
    framerate_layer.set_font("/Users/ldonovan/Library/Fonts/Blex Mono Nerd Font Complete Mono-1.ttf", 14);
#else
    framerate_layer.set_font("/home/luke/.local/share/fonts/Blex Mono Nerd Font Complete Mono.ttf", 14);
#endif

    framerate_layer.set_position(0.0f, 0.0f);
}

void Engine::handle_key_event(int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        this->add_outgoing_event({LayerUpdateRequest, {}});
    } else if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        this->add_outgoing_event({LayerUpdateRequest, {}});
    }
}

void Engine::process_resource_requests() {
    std::optional<resource_request_t> request;
    while ((request = entity_layer.pop_resource_request()).has_value()) {
        switch (request->type) {
        case Mesh:
            this->add_outgoing_event({MeshLoadRequest, {&entity_layer, request->name}});
            break;
        case Texture:
            this->add_outgoing_event({TextureLoadRequest, {&entity_layer, request->name}});
            break;
        default:
            PLOGE << "Got unknown resource request type";
            break;
        }
    }
}

void Engine::process_events() {
    // This is where the logic is actually handled
    // 1. Process incoming events from the window
    // 2. Send outgoing events to the window

    std::optional<Event> event;
    while ((event = this->pop_incoming_event()).has_value()) {
        switch (event->type) {
        case WindowResize:
            PLOGI << "Got window resize event";
            PLOGI << "W: " << INT(*event, 0) << ", H: " << INT(*event, 1);
            this->add_outgoing_event({LayerUpdateRequest, {}});
            break;
        case Key:
            this->handle_key_event(INT(*event, 0), INT(*event, 1), INT(*event, 2), INT(*event, 3));
            break;
        case MeshLoad:
            LAYER(*event, 0)->receive_resource(Mesh, STRING(*event, 1), VOID(*event, 2));
            break;
        case TextureLoad:
            LAYER(*event, 0)->receive_resource(Texture, STRING(*event, 1), VOID(*event, 2));
            break;
        case CursorPosition:
            framerate_layer.set_position(
                DOUBLE(*event, 0),
                DOUBLE(*event, 1)
            );
            break;
        case Tick1:
        case Tick10:
        case Tick100:
        case Framerate:
            break;
        default:
            PLOGW << "Got unknown event type";
            break;
        }
    }
}

std::optional<Event> Engine::pop_incoming_event() {
    if (this->incoming_event_queue.empty())
        return {};

    Event event = this->incoming_event_queue.front();
    this->incoming_event_queue.pop();
    return event;
}

void Engine::add_outgoing_event(Event event) {
    this->outgoing_event_queue.push(event);
}

std::optional<Event> Engine::pop_outgoing_event() {
    if (this->outgoing_event_queue.empty())
        return {};

    Event event = this->outgoing_event_queue.front();
    this->outgoing_event_queue.pop();
    return event;
}

void Engine::add_incoming_event(Event event) {
    this->incoming_event_queue.push(event);
}
