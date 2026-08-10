#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/ColorSelectLiveOverlay.hpp>
#include <Geode/modify/HSVLiveOverlay.hpp>

class $module(UIScaling) {
    bool onToggled(bool state);
    bool onSettingChanged(std::string_view key, const matjson::Value& value);
    void onEditor();
    void setScaling(bool fullReload);
    void setPauseScaling();

    void resetSettings();
    void setupSettings();
    void setupEvents();

    static CCPoint getSafeOffset();

    float m_scale = 1.f;
    float m_customSafeArea = 0.f;
    bool m_scaleToolbar = true;
    bool m_scalePause = true;
    bool m_usesSafeArea = true;
    bool m_usesCustomSafeArea = false;
};

class $modify(UISColorSelectLiveOverlay, ColorSelectLiveOverlay) {
    $registerHooks(UIScaling)

    bool init(ColorAction* baseAction, ColorAction* detailAction, EffectGameObject* object);

    static void scaleOverlay(ColorSelectLiveOverlay* overlay);
    static void scaleActive();
};

class $modify(UISHSVLiveOverlay, HSVLiveOverlay) {
    $registerHooks(UIScaling)

    bool init(GameObject* object, cocos2d::CCArray* objects);

    static void scaleOverlay(HSVLiveOverlay* overlay);
    static void scaleActive();
};

// evil global hook for BE compat
class $modify(UISEditorUI, EditorUI) {
    bool init(LevelEditorLayer* editorLayer);

    static void onModify(auto& self) {
        (void) self.setHookPriorityPost("EditorUI::init", Priority::VeryLate);
    }
};
