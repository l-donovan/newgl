#pragma once

#include "event.h"
#include "safe_queue.h"

#include <optional>
#include <string>
#include <queue>

// Controller is designed to isolate entirely from the Application system,
// as declared in application.h
class Controller {
    private:
        // Internal handlers
        void handle_key_event(int key, int scancode, int action, int mods);
    public:
        SafeQueue<Event> incoming_events;
        SafeQueue<Event> outgoing_events;

        virtual void pre_application_startup() = 0;
        virtual void post_application_startup() = 0;
        virtual void process_resource_requests() = 0;

        virtual void process_events(bool single_pass) = 0;
};