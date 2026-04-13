#pragma once

#include "../Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $editorModule(AutoBuildHelper) {
    CCMenuItemToggler* m_bhToggler;
    bool m_autoBuildHelperEnabled = false;

    void onEditor() override;
    void onEditorPauseLayer(EditorPauseLayer* editorPauseLayer) override;

    void removeFromEditorUI();
    void removeFromPause();

    void showOnEditorUI();
    void showOnPause();

    bool onToggled(bool state) override;
    bool onSettingChanged(std::string_view key, const matjson::Value& value) override;

    void onToggleAutoBuildHelper(CCObject* sender);
};

class $modify(ABHEditorUI, EditorUI) {
    $registerEditorHooks(AutoBuildHelper)

    CCArray* pasteObjects(gd::string str, bool withColor, bool noUndo);
};