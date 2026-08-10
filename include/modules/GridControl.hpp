#pragma once

#ifndef GEODE_IS_ANDROID32

#include "module/Module.hpp"
#include <Geode/modify/ObjectToolbox.hpp>

class $module(GridControl) {
    TextInput* m_input;
    Ref<CCNode> m_oldBEControl;

    void onEditor();
    void updateGrid(float newValue = 0, bool updateInput = true);
    static float getSliderMinY(EditorUI* editorUI);

    tinker::utils::ScopedHookToggle<"hjfod.betteredit", GridControl> m_toggledHooks = {
        "EditorUI::updateGridNodeSize"
    };
};

class $modify(GCObjectToolbox, ObjectToolbox) {
    $registerHooks(GridControl)

    float gridNodeSizeForKey(int id);
};

#endif