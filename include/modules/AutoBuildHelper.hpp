#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $module(AutoBuildHelper) {
    CCMenuItemToggler* m_bhToggler;
    bool m_autoBuildHelperEnabled = false;

    void onEditor();

    void removeFromEditorUI();
    void removeFromPause();

    void showOnEditorUI();
    void showOnPause();

    bool onToggled(bool state);
    bool onSettingChanged(std::string_view key, const matjson::Value& value);

    void onToggleAutoBuildHelper(CCObject* sender);
};

class $modify(ABHEditorUI, EditorUI) {
    $registerHooks(AutoBuildHelper)

    CCArray* pasteObjects(gd::string str, bool withColor, bool noUndo);
};