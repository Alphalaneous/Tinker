#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

class $module(MusicCameraLock) {
    bool m_cameraLocked = false;
    geode::Ref<CCMenuItemToggler> m_toggler = nullptr;

    bool onToggled(bool state);
    void onEditor();
};

class $modify(MCLEditorUI, EditorUI) {
    $registerHooks(MusicCameraLock)

    void showLayoutGenerator(cocos2d::CCNode* menu, bool show);

    void onPlayback(cocos2d::CCObject* sender);
    void onPlaytest(cocos2d::CCObject* sender);
};

class $modify(MCLLevelEditorLayer, LevelEditorLayer) {
    $registerHooks(MusicCameraLock)

    void lockCamera(float dt);
    void onStopPlaytest();
};