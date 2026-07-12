#pragma once

#ifndef GEODE_IS_ANDROID32

#include "module/Module.hpp"
#include <Geode/modify/ObjectToolbox.hpp>

class $editorModule(GridControl) {
    TextInput* m_input;
    Ref<CCNode> m_oldBEControl;

    void onEditor();
    void updateGrid(float newValue = 0, bool updateInput = true);
    static float getSettingsMenuWidth(EditorUI* editorUI);
    static float getSliderMaxX(EditorUI* editorUI);
    static float getSliderMinY(EditorUI* editorUI);

    GridControl();
    virtual ~GridControl();
};

class $modify(GCObjectToolbox, ObjectToolbox) {
    $registerEditorHooks(GridControl)

    float gridNodeSizeForKey(int id);
};

#endif