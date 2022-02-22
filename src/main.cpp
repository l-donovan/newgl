#include "newgl/global.h"
#include "newgl/engine.h"
#include "newgl/window.h"

// Create window and engine objects
Window window("newgl", 1200, 800);
Engine engine;

// This prevents some linking issues in Windows
#undef main

int main(int argc, char **argv) {
    // Initialize our logger
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(plog::debug, &consoleAppender);

    // Attach the engine to the window
    window.attach(&engine);

    // Do some things before starting up the window
    engine.pre_window_startup();

    // Startup the window, compiling the shaders and initializing buffers
    window.startup();

    // It is then safe to startup the engine since the window's dimensions
    // have been determined, shaders can be compiled, etc.
    engine.post_window_startup();

    // Start the window's main loop
    window.main_loop();

    return EXIT_SUCCESS;
}
