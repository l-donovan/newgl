#include "newgl/global.h"
#include "newgl/common.h"
#include "newgl/engine.h"
#include "newgl/event.h"
#include "newgl/shader.h"
#include "newgl/window.h"

#include "newgl/example_layer.h"
#include "newgl/entity_layer.h"
#include "newgl/text_layer.h"
#include "newgl/sky_layer.h"
#include "newgl/script_layer.h"

#include "newgl/attributes/capture_framerate.h"
#include "newgl/attributes/first_person_camera.h"
#include "newgl/attributes/basic_motion.h"
#include "newgl/attributes/toggle_wireframe.h"

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

Shader sky_shader(
    ROOT_DIR + "/resources/shaders/sky.v.glsl",
    ROOT_DIR + "/resources/shaders/sky.f.glsl");

EntityLayer suzanne;
EntityLayer cactus;
TextLayer framerate_layer;
SkyLayer skybox;
ScriptLayer script_layer;

Engine::Engine() {
    framerate_layer.add_attribute(std::make_shared<CaptureFramerate>(&framerate_layer));

    suzanne.request_resource(Texture, ROOT_DIR + "/resources/images/chinese_garden.png");
    suzanne.request_resource(Mesh, ROOT_DIR + "/resources/models/suzanne.obj");
    suzanne.add_attribute(std::make_shared<BasicMotion>(&suzanne));
    suzanne.add_attribute(std::make_shared<ToggleWireframe>(&suzanne));

    cactus.request_resource(Texture, ROOT_DIR + "/resources/images/cactus.png");
    cactus.request_resource(Mesh, ROOT_DIR + "/resources/models/cactus.obj");
    cactus.add_attribute(std::make_shared<ToggleWireframe>(&cactus));
    cactus.set_scale(4.0f);

    skybox.request_resource(Texture, ROOT_DIR + "/resources/images/chinese_garden.png");

    script_layer.add_attribute(std::make_shared<FirstPersonCamera>());
}

Engine::~Engine() {}

void Engine::pre_window_startup() {
    // We have to send some events to the window to setup our layers and shaders

    this->outgoing_events.enqueue({LayerModifyRequest, {
        EVENT_LAYER_ADD,
        &suzanne,
        &base_shader
    }});

    this->outgoing_events.enqueue({LayerModifyRequest, {
        EVENT_LAYER_ADD,
        &cactus,
        &base_shader
    }});

    this->outgoing_events.enqueue({LayerModifyRequest, {
        EVENT_LAYER_ADD,
        &framerate_layer,
        &text_shader
    }});

    this->outgoing_events.enqueue({LayerModifyRequest, {
        EVENT_LAYER_ADD,
        &skybox,
        &sky_shader
    }});

    this->outgoing_events.enqueue({LayerModifyRequest, {
        EVENT_LAYER_ADD_BLANK,
        &script_layer
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
}

void Engine::handle_key_event(int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        this->outgoing_events.enqueue({LayerUpdateRequest, {}});
    } else if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        this->outgoing_events.enqueue({LayerUpdateRequest, {}});
    }
}

void Engine::process_resource_requests() {
    std::optional<resource_request_t> request;

    for (auto layer : {(Layer*) &suzanne, (Layer*) &cactus, (Layer*) &framerate_layer, (Layer*) &skybox}) { // TODO: This is awful
        while ((request = layer->pop_resource_request()).has_value()) {
            switch (request->type) {
            case Mesh:
                this->outgoing_events.enqueue({MeshLoadRequest, {layer, request->name}});
                break;
            case Texture:
                this->outgoing_events.enqueue({TextureLoadRequest, {layer, request->name}});
                break;
            default:
                PLOGW << "Got unknown resource request type";
                break;
            }
        }
    }
}

void Engine::process_events(Engine *engine, std::atomic<bool> *run_flag, bool single_pass) {
    // This is where the logic is actually handled
    // 1. Process incoming events from the window
    // 2. Send outgoing events to the window

    Event event;

    while (!single_pass || !engine->incoming_events.empty()) {
        event = engine->incoming_events.dequeue();

        switch (event.type) {
        case WindowResize:
            PLOGI << "Got window resize event";
            PLOGI << "W: " << INT(0) << ", H: " << INT(1);
            engine->outgoing_events.enqueue({LayerUpdateRequest, {}});
            break;
        case Key:
            engine->handle_key_event(INT(0), INT(1), INT(2), INT(3));
            break;
        case MeshLoad:
            LAYER(0)->receive_resource(Mesh, STRING(1), VOID(2));
            break;
        case TextureLoad:
            LAYER(0)->receive_resource(Texture, STRING(1), VOID(2));
            break;
        case Break:
            PLOGI << "Got Break event. Exiting";
            return;
        case CursorPosition:
        case Tick1:
        case Tick10:
        case Tick100:
        case Framerate:
        case BeginDraw:
        case EndDraw:
            break;
        default:
            PLOGW << "Got unknown event type";
            break;
        }
    }
}
