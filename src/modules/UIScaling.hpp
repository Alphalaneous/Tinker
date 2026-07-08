#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $editorModule(UIScaling) {
    bool onToggled(bool state) override;
    bool onSettingChanged(std::string_view key, const matjson::Value& value) override;
    void onEditor() override;
    void setScaling(float scale, bool toolbar, bool topAlign, bool fullReload);
    void setPauseScaling(float scale);

    void setupEvents();

    static CCPoint getSafeOffset();

    static float getUIScale(bool ignoreEnabled = false);
    static bool shouldScaleToolbar();
    static bool shouldScalePause();
};

// evil global hook for BE compat
class $modify(UISEditorUI, EditorUI) {
    bool init(LevelEditorLayer* editorLayer);

    static void onModify(auto& self) {
        (void) self.setHookPriorityAfterPost("EditorUI::init", "hjfod.betteredit");
    }
};