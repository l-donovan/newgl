#include "demo/demo_controller.h"

#include "engine/global.h"
#include "engine/common.h"
#include "engine/layer.h"
#include "engine/material.h"
#include "engine/shader.h"

#include "engine/entity_layer.h"
#include "engine/script_layer.h"
#include "engine/sky_layer.h"
#include "engine/text_layer.h"

#include "engine/attributes/capture_framerate.h"
#include "engine/attributes/first_person_camera.h"
#include "engine/attributes/basic_motion.h"
#include "engine/attributes/toggle_wireframe.h"

// `_ROOT_DIR` is set via cmake
std::string ROOT_DIR(_ROOT_DIR);

Shader base_shader(
    ROOT_DIR + "/demo/resources/shaders/base.v.glsl",
    ROOT_DIR + "/demo/resources/shaders/base.f.glsl");

Shader text_shader(
    ROOT_DIR + "/demo/resources/shaders/text.v.glsl",
    ROOT_DIR + "/demo/resources/shaders/text.f.glsl");

Shader sky_shader(
    ROOT_DIR + "/demo/resources/shaders/sky.v.glsl",
    ROOT_DIR + "/demo/resources/shaders/sky.f.glsl");

EntityLayer suzanne;
EntityLayer cactus;
TextLayer framerate_layer;
SkyLayer skybox;
ScriptLayer script_layer;

Material suzanne_mat;
Material cactus_mat;

DemoController::DemoController() {
    framerate_layer.add_attribute(std::make_shared<CaptureFramerate>(&framerate_layer));

    suzanne.request_resource(Texture, ROOT_DIR + "/demo/resources/images/chinese_garden.png");
    suzanne.request_resource(MeshResource, ROOT_DIR + "/demo/resources/models/suzanne.obj");
    suzanne.add_attribute(std::make_shared<BasicMotion>(&suzanne));
    suzanne.add_attribute(std::make_shared<ToggleWireframe>(&suzanne));

    suzanne_mat.set("color", glm::vec3(0.6, 0.5, 0.6));
    suzanne_mat.set("metallic", 1.0f);
    suzanne_mat.set("roughness", 1.0f);
    suzanne_mat.set("reflectance", 0.5f);
    suzanne_mat.set("clear_coat", 0.1f);
    suzanne_mat.set("clear_coat_roughness", 0.1f);

    suzanne.material = &suzanne_mat;

    cactus.request_resource(Texture, ROOT_DIR + "/demo/resources/images/cactus.png");
    cactus.request_resource(MeshResource, ROOT_DIR + "/demo/resources/models/cactus.obj");
    cactus.add_attribute(std::make_shared<ToggleWireframe>(&cactus));
    cactus.set_scale(4.0f);
    cactus.set_position(-1.0f, 1.0f, 1.0f);

    cactus_mat.set("color", glm::vec3(0.6, 0.5, 0.6));
    cactus_mat.set("metallic", 0.0f);
    cactus_mat.set("roughness", 0.1f);
    cactus_mat.set("reflectance", 0.5f);
    cactus_mat.set("clear_coat", 0.1f);
    cactus_mat.set("clear_coat_roughness", 0.1f);

    cactus.material = &cactus_mat;

    skybox.request_resource(Texture, ROOT_DIR + "/demo/resources/images/chinese_garden.png");

    script_layer.add_attribute(std::make_shared<FirstPersonCamera>());
}

DemoController::~DemoController() {

}

void DemoController::pre_application_startup() {
    // We have to send some events to the application to setup our layers and shaders

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

void DemoController::post_application_startup() {
    framerate_layer.set_font(ROOT_DIR + "/demo/resources/fonts/Inconsolata.ttf", 18); // TODO: This should be handled as a resource
}

void DemoController::process_resource_requests() {
    std::optional<resource_request_t> request;

    for (auto layer : {(Layer*) &suzanne, (Layer*) &cactus, (Layer*) &framerate_layer, (Layer*) &skybox}) { // TODO: This is awful
        while ((request = layer->pop_resource_request()).has_value()) {
            switch (request->type) {
            case MeshResource:
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

void DemoController::handle_key_event(int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        this->outgoing_events.enqueue({LayerUpdateRequest, {}});
    } else if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        this->outgoing_events.enqueue({LayerUpdateRequest, {}});
    }
}

void DemoController::process_events(bool single_pass) {
    // This is where the logic is actually handled
    // 1. Process incoming events from the application
    // 2. Send outgoing events to the application

    Event event;

    while (!single_pass || !this->incoming_events.empty()) {
        event = this->incoming_events.dequeue();

        switch (event.type) {
        case WindowResize:
            PLOGI << "Got window resize event";
            PLOGI << "W: " << INT(0) << ", H: " << INT(1);
            this->outgoing_events.enqueue({LayerUpdateRequest, {}});
            break;
        case Key:
            this->handle_key_event(INT(0), INT(1), INT(2), INT(3));
            break;
        case MeshLoad:
            LAYER(0)->receive_resource(MeshResource, STRING(1), VOID(2));
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
