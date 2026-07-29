#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $editorModule(ImprovedLayers) {
    void onEditor();
};

class $modify(ILEditorUI, EditorUI) {
    $registerEditorHooks(ImprovedLayers)

};