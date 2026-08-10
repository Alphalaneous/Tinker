#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $module(MusicCameraLock) {
    bool m_cameraLocked = false;
    Ref<CCMenuItemToggler> m_toggler = nullptr;

    bool onToggled(bool state);
    void onEditor();
};

class $modify(MCLEditorUI, EditorUI) {
    $registerHooks(MusicCameraLock)

    void onPlayback(CCObject* sender);
    void onPlaytest(CCObject* sender);

    void lockCamera(float dt);
};