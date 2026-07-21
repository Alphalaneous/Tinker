#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $editorModule(NoTouchPasteStateFix) {
    bool onToggled(bool state) override;

    void fixNoTouch(GameObject* object, CCArray* objects);
};

class $modify(NTPSFEditorUI, EditorUI) {
    $registerEditorHooks(NoTouchPasteStateFix)

    void onPasteState(cocos2d::CCObject* sender);
};