#pragma once

#ifndef GEODE_IS_ANDROID32

#include "module/Module.hpp"
#include <Geode/modify/ObjectToolbox.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

class $module(GridControlOld) {
    TextInput* m_input;
    Ref<CCNode> m_control;
    Ref<CCMenuItemToggler> m_toggler;
    bool m_addedCallbacks;
    bool m_gridScaleToggled;

    bool onToggled(bool state);
    bool onSettingChanged(std::string_view key, const matjson::Value& value);

    void onEditor();
    void updateUI(float scale);
    void updateGrid(float newValue = 0, bool updateInput = true);
    float getGridMultiplier();

    static float getSliderMinY(EditorUI* editorUI);
};

class $modify(GCObjectToolbox, ObjectToolbox) {
    $registerHooks(GridControlOld)

    float gridNodeSizeForKey(int id);
};

class $modify(GCEditorUI, EditorUI) {
    $registerHooks(GridControlOld)

    cocos2d::CCPoint offsetForKey(int id);
    GameObject* createObject(int objectID, cocos2d::CCPoint position);
};

class $modify(GCLevelEditorLayer, LevelEditorLayer) {
    $registerHooks(GridControlOld)
};

#endif