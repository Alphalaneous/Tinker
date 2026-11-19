#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/ColorSelectLiveOverlay.hpp>
#include <Geode/modify/HSVLiveOverlay.hpp>

class $module(UIScaling) {
    class UIScaleSettingsChanged : public geode::Event<UIScaleSettingsChanged, bool()> {
        using Event::Event;
    };

    bool onSettingChanged(std::string_view key, const matjson::Value& value);
    void onEditor();
    void setScaling();
    void setPauseScaling();

    void updateUIScalingMod();

    void setupSettings();
    void setupEvents();

    static cocos2d::CCPoint getSafeOffset();
    static cocos2d::CCPoint getSafeOffsetPause();
    static float getScale();
    static float getToolbarScale();
    static float getPauseScale();

    static bool waitForUIScale();

    float m_scale = 1.f;
    float m_customSafeArea = 0.f;
    bool m_scaleToolbar = true;
    bool m_scalePause = true;
    bool m_usesSafeArea = true;
    bool m_usesSafeAreaPause = true;
    bool m_usesCustomSafeArea = false;

    bool m_uiScaleStarted = false;
};

class $modify(UISColorSelectLiveOverlay, ColorSelectLiveOverlay) {
    $registerHooks(UIScaling)

    bool init(ColorAction* baseAction, ColorAction* detailAction, EffectGameObject* object);

    static void scaleOverlay(ColorSelectLiveOverlay* overlay);
    static void scaleActive();
};

class $modify(UISHSVLiveOverlay, HSVLiveOverlay) {
    $registerHooks(UIScaling)

    bool init(GameObject* object, cocos2d::CCArray* objects, cocos2d::ccColor3B p2, cocos2d::ccColor3B p3);

    static void scaleOverlay(HSVLiveOverlay* overlay);
    static void scaleActive();
};

class $modify(UISEditorUI, EditorUI) {

    bool init(LevelEditorLayer* editorLayer);

    static void onModify(auto& self) {
        (void) self.setHookPriorityPost("EditorUI::init", geode::Priority::VeryLate);
    }
};