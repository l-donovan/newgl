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
};