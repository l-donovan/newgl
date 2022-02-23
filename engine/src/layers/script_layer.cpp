#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <plog/Log.h>

#include "engine/common.h"
#include "engine/global.h"
#include "engine/script_layer.h"

using std::string;

ScriptLayer::ScriptLayer() {}

void ScriptLayer::receive_resource(ResourceType type, string name, void *data) {}
void ScriptLayer::setup() {}
void ScriptLayer::update() {}
void ScriptLayer::draw(glm::mat4 view, glm::mat4 projection, camera_t camera) {}
void ScriptLayer::teardown() {}