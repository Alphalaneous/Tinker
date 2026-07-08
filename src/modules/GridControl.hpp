#pragma once

#include "module/Module.hpp"
#include <Geode/modify/ObjectToolbox.hpp>
#include <Geode/modify/EditorUI.hpp>

class $editorModule(GridControl) {
    TextInput* m_input;

    void onEditor();
    void updateGrid(float newValue = 0, bool updateInput = true);
};

class $modify(GCObjectToolbox, ObjectToolbox) {
    $registerEditorHooks(GridControl)

    float gridNodeSizeForKey(int id);
};

class $modify(GCEditorUI, EditorUI) {
    $registerEditorHooks(GridControl)

    void updateGridNodeSize();
};