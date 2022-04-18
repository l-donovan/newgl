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
#include "engine/layers/instance_layer.h"

#include "demo/attributes/capture_framerate.h"
#include "demo/attributes/first_person_camera.h"
#include "demo/attributes/basic_motion.h"
#include "demo/attributes/toggle_wireframe.h"
#include "demo/attributes/movement_controller.h"
#include "demo/attributes/grab_input.h"
#include "demo/attributes/material_adapter.h"

#include "demo/layers/material_editor.h"

// `_ROOT_DIR` is set via cmake
std::string ROOT_DIR(_ROOT_DIR);

Shader phong_shader(
    ROOT_DIR + "/demo/resources/shaders/phong.v.glsl",
    ROOT_DIR + "/demo/resources/shaders/phong.f.glsl");

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

bool instance_handler(int num) {
    // Skip every 13th suzanne, just for fun
    if (num % 13 == 0) {
        return false;
    }

    int row = num / 50;
    int col = num % 50;
    float dist = sqrtf(row * row + col * col);

    suzanne.set_position(1.5f * row, sinf(dist), 1.5f * col);

    return true;
}

InstanceLayer<EntityLayer> suzanne_instances(2500, &suzanne, instance_handler);

DemoController::DemoController() {
    ADD_ATTRIBUTE(framerate_layer, CaptureFramerate);

    suzanne.request_resource(Texture, ROOT_DIR + "/demo/resources/images/chinese_garden.png");
    suzanne.request_resource(MeshResource, ROOT_DIR + "/demo/resources/models/suzanne.obj");
    ADD_ATTRIBUTE(suzanne, BasicMotion);
    ADD_ATTRIBUTE(suzanne, ToggleWireframe);

    suzanne_mat.set("color", glm::vec3(0.6, 0.5, 0.6));
    suzanne_mat.set("use_texture", 0);

    suzanne.material = &suzanne_mat;

    cactus.request_resource(Texture, ROOT_DIR + "/demo/resources/images/cactus.png");
    cactus.request_resource(MeshResource, ROOT_DIR + "/demo/resources/models/cactus.obj");
    cactus.set_scale(4.0f);
    cactus.set_position(-1.0f, 1.0f, 1.0f);
    ADD_ATTRIBUTE(cactus, ToggleWireframe);

    cactus_mat.set("color", glm::vec3(0.6, 0.5, 0.6));
    cactus_mat.set("use_texture", 1);

    cactus.material = &cactus_mat;

    skybox.request_resource(Texture, ROOT_DIR + "/demo/resources/images/chinese_garden.png");

    ADD_GENERIC_ATTRIBUTE(script_layer, FirstPersonCamera);
    ADD_GENERIC_ATTRIBUTE(script_layer, MovementController);

    ADD_GENERIC_ATTRIBUTE(material_editor, GrabInput);
    ADD_ATTRIBUTE(material_editor, MaterialAdapter, &suzanne_mat);
    ADD_ATTRIBUTE(material_editor, MaterialAdapter, &cactus_mat);
}

DemoController::~DemoController() {

}

void DemoController::pre_application_startup() {
    // We have to send some events to the application to setup our layers and shaders

    ADD_LAYER(suzanne_instances, phong_shader);
    ADD_LAYER(cactus, phong_shader);
    ADD_LAYER(framerate_layer, text_shader);
    ADD_LAYER(skybox, sky_shader);
    ADD_BLANK_LAYER(script_layer);

    // TODO: The sky_shader is arbitrary. We shouldn't need a shader for a draw call
    ADD_LAYER(material_editor, sky_shader);
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
