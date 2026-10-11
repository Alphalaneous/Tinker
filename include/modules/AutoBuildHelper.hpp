#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $module(AutoBuildHelper) {
    CCMenuItemToggler* m_bhToggler = nullptr;
    bool m_autoBuildHelperEnabled = false;

    void onEditor();

    void removeFromEditorUI();
    void removeFromPause();

    void showOnEditorUI();
    void showOnPause();

    bool onToggled(bool state);
    bool onSettingChanged(std::string_view key, const matjson::Value& value);

    void onToggleAutoBuildHelper(cocos2d::CCObject* sender);
};

class $modify(ABHEditorUI, EditorUI) {
    $registerHooks(AutoBuildHelper)

    cocos2d::CCArray* pasteObjects(gd::string str, bool withColor, bool noUndo);
};