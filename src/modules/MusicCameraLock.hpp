#pragma once

#include "../Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $editorModule(MusicCameraLock) {
    bool m_cameraLocked = false;
    Ref<CCMenuItemToggler> m_toggler = nullptr;

    bool onToggled(bool state) override;
    void onEditor() override;
};

class $modify(MCLEditorUI, EditorUI) {
    $registerEditorHooks(MusicCameraLock)

    void onPlayback(CCObject* sender);
    void onPlaytest(CCObject* sender);

    void lockCamera(float dt);
};