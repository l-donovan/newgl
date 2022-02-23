#pragma once

#include "engine/controller.h"

class DemoController : public Controller {
    public:
        DemoController();
        ~DemoController();

        void pre_application_startup();
        void post_application_startup();
        void process_resource_requests();

        void process_events(bool single_pass);
        void handle_key_event(int key, int scancode, int action, int mods);
};