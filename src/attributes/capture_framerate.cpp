#include "newgl/attributes/capture_framerate.h"

#include "newgl/global.h"
#include "newgl/common.h"
#include "newgl/event.h"

#include <string>

void CaptureFramerate::handle_framerate(float fps, float percent) {
    std::ostringstream text;
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
    case Framerate:
        this->handle_framerate(FLOAT(event, 0), FLOAT(event, 1));
        break;
    default:
        break;
    }
}
