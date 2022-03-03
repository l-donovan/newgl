#pragma once

#include "engine/attribute.h"
#include "engine/event.h"
#include "engine/material.h"

#include "demo/layers/material_editor.h"

class MaterialAdapter : public Attribute, public std::enable_shared_from_this<MaterialAdapter> {
    private:
        void handle_tick10();

        MaterialEditor *editor;
        Material *material;
    public:
        MaterialAdapter(MaterialEditor *editor, Material *material) : editor(editor), material(material) {
            this->subscribe(EventType::Tick10);
        };

        void receive_event(Event event);

        std::shared_ptr<MaterialAdapter> getptr() {
            return shared_from_this();
        }
};