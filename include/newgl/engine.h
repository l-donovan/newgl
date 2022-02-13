#pragma once

#include "event.h"

#include <optional>
#include <string>
#include <queue>

// Engine is designed to isolate entirely from the windowing system,
// as declared in window.h
class Engine {
    private:
        std::queue<Event> incoming_event_queue;
        std::queue<Event> outgoing_event_queue;

        std::optional<Event> pop_incoming_event();

        // Internal handlers
        void handle_key_event(int key, int scancode, int action, int mods);
    public:
        Engine();
        ~Engine();

        void pre_window_startup();
        void post_window_startup();
        void process_events();
        void process_resource_requests();

        void add_incoming_event(Event event);
        void add_outgoing_event(Event event);
        std::optional<Event> pop_outgoing_event();
};
