#include "demo/demo_controller.h"

#include "engine/global.h"
#include "engine/common.h"
#include "engine/layer.h"
#include "engine/material.h"
#include "engine/shader.h"

#include "engine/layers/entity_layer.h"
#include "engine/layers/script_layer.h"
#include "engine/layers/sky_layer.h"
#include "engine/layers/text_layer.h"

#include "demo/attributes/capture_framerate.h"
#include "demo/attributes/first_person_camera.h"
#include "demo/attributes/basic_motion.h"
#include "demo/attributes/toggle_wireframe.h"
#include "demo/attributes/movement_controller.h"
#include "demo/attributes/toggle_cursor.h"
#include "demo/attributes/grab_input.h"
#include "demo/attributes/material_adapter.h"

#include "demo/layers/material_editor.h"

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
MaterialEditor material_editor;

Material suzanne_mat;
Material cactus_mat;

DemoController::DemoController() {
    ADD_ATTRIBUTE(framerate_layer, CaptureFramerate);

    suzanne.request_resource(Texture, ROOT_DIR + "/demo/resources/images/chinese_garden.png");
    suzanne.request_resource(MeshResource, ROOT_DIR + "/demo/resources/models/suzanne.obj");
    ADD_ATTRIBUTE(suzanne, BasicMotion);
    ADD_ATTRIBUTE(suzanne, ToggleWireframe);

    suzanne_mat.set("color", glm::vec3(0.6, 0.5, 0.6));
    suzanne_mat.set("metallic", 1.0f);
    suzanne_mat.set("roughness", 1.0f);
    suzanne_mat.set("reflectance", 0.5f);
    suzanne_mat.set("clear_coat", 0.1f);
    suzanne_mat.set("clear_coat_roughness", 0.1f);

    suzanne.material = &suzanne_mat;

    cactus.request_resource(Texture, ROOT_DIR + "/demo/resources/images/cactus.png");
    cactus.request_resource(MeshResource, ROOT_DIR + "/demo/resources/models/cactus.obj");
    cactus.set_scale(4.0f);
    cactus.set_position(-1.0f, 1.0f, 1.0f);
    ADD_ATTRIBUTE(cactus, ToggleWireframe);

    cactus_mat.set("color", glm::vec3(0.6, 0.5, 0.6));
    cactus_mat.set("metallic", 0.0f);
    cactus_mat.set("roughness", 0.1f);
    cactus_mat.set("reflectance", 0.5f);
    cactus_mat.set("clear_coat", 0.1f);
    cactus_mat.set("clear_coat_roughness", 0.1f);

    cactus.material = &cactus_mat;

    skybox.request_resource(Texture, ROOT_DIR + "/demo/resources/images/chinese_garden.png");

    ADD_GENERIC_ATTRIBUTE(script_layer, FirstPersonCamera);
    ADD_GENERIC_ATTRIBUTE(script_layer, MovementController);
    ADD_GENERIC_ATTRIBUTE(script_layer, ToggleCursor);

    ADD_GENERIC_ATTRIBUTE(material_editor, GrabInput);
    ADD_ATTRIBUTE(material_editor, MaterialAdapter, &suzanne_mat);
}

DemoController::~DemoController() {

}

void DemoController::pre_application_startup() {
    // We have to send some events to the application to setup our layers and shaders

    this->outgoing_events.enqueue({EventType::LayerModifyRequest, {
        EVENT_LAYER_ADD,
        &suzanne,
        &base_shader
    }});

    this->outgoing_events.enqueue({EventType::LayerModifyRequest, {
        EVENT_LAYER_ADD,
        &cactus,
        &base_shader
    }});

    this->outgoing_events.enqueue({EventType::LayerModifyRequest, {
        EVENT_LAYER_ADD,
        &framerate_layer,
        &text_shader
    }});

    this->outgoing_events.enqueue({EventType::LayerModifyRequest, {
        EVENT_LAYER_ADD,
        &skybox,
        &sky_shader
    }});

    this->outgoing_events.enqueue({EventType::LayerModifyRequest, {
        EVENT_LAYER_ADD_BLANK,
        &script_layer
    }});

    // TODO: The sky_shader is arbitrary. We shouldn't need a shader for a draw call
    this->outgoing_events.enqueue({EventType::LayerModifyRequest, {
        EVENT_LAYER_ADD,
        &material_editor,
        &sky_shader
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
                this->outgoing_events.enqueue({EventType::MeshLoadRequest, {layer, request->name}});
                break;
            case Texture:
                this->outgoing_events.enqueue({EventType::TextureLoadRequest, {layer, request->name}});
                break;
            default:
                PLOGW << "Got unknown resource request type";
                break;
            }
        }
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
        case EventType::WindowResize:
            PLOGI << "Got window resize event";
            PLOGI << "W: " << INT(0) << ", H: " << INT(1);
            this->outgoing_events.enqueue({EventType::LayerUpdateRequest, {}});
            break;
        case EventType::MeshLoad:
            LAYER(0)->receive_resource(MeshResource, STRING(1), VOID(2));
            break;
        case EventType::TextureLoad:
            LAYER(0)->receive_resource(Texture, STRING(1), VOID(2));
            break;
        case EventType::Break:
            PLOGI << "Got Break event. Exiting";
            return;
        default:
            break;
        }
    }
}
