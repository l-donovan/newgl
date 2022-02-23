#include "engine/global.h"
#include "engine/application.h"

#include "demo/demo_controller.h"

// Create application and controller objects
Application application("engine", 1200, 800);
DemoController controller;

// This prevents some linking issues in Windows
#undef main

int main(int argc, char **argv) {
    // Initialize our logger
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(plog::debug, &consoleAppender);

    // Attach the controller to the application
    application.attach(&controller);

    // Do some things before starting up the application
    controller.pre_application_startup();

    // Startup the application, compiling the shaders and initializing buffers
    application.startup();

    // It is then safe to startup the controller. Since the application's dimensions
    // have been determined, shaders can be compiled, etc.
    controller.post_application_startup();

    // Start the application's main loop
    application.main_loop();

    return EXIT_SUCCESS;
}
