#define GEODE_DEFINE_EVENT_EXPORTS
#include "../include/API.hpp"

#include <Geode/Geode.hpp>
#include "InputsHandler.hpp"

using namespace geode::prelude;

namespace tinker::api {
    namespace touch {
        void blockPinch(bool block) {
            auto editorUI = InputEditorUI::get();
            if (!editorUI) return;

            editorUI->blockPinch(block);
        }
    }
}