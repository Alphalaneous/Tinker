#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $module(ImprovedLayers) {
    void onEditor();
};

class $modify(ILEditorUI, EditorUI) {
    $registerHooks(ImprovedLayers)

};