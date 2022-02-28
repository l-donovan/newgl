#include "demo/attributes/capture_framerate.h"

#include "engine/global.h"
#include "engine/common.h"
#include "engine/event.h"

#include <string>

void CaptureFramerate::handle_framerate(float fps, float percent) {
    std::ostringstream text;
    text << "FPS Info\n";
    text << "--------\n";
    text << "FPS: " << fps << ", ";

    if (percent > 0)
        text << "ahead " << percent;
    else
        text << "behind " << -percent;

    text << "%";

    this->text_layer->set_text(text.str());
}

void CaptureFramerate::receive_event(Event event) {
    switch (event.type) {
    case EventType::Framerate:
        this->handle_framerate(FLOAT(0), FLOAT(1));
        break;
    default:
        break;
    }
}
