#pragma once

#include "event.h"
#include "safe_queue.h"

#include <optional>
#include <string>
#include <queue>

// Engine is designed to isolate entirely from the windowing system,
// as declared in window.h
class Engine {
    private:
        // Internal handlers
        void handle_key_event(int key, int scancode, int action, int mods);
    public:
        Engine();
        ~Engine();

        SafeQueue<Event> incoming_events;
        SafeQueue<Event> outgoing_events;

        void pre_window_startup();
        void post_window_startup();
        void process_resource_requests();

        static void process_events(Engine *engine, std::atomic<bool> *run_flag, bool single_pass);
};
