#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $module(NoTouchPasteStateFix) {
    bool onToggled(bool state);

    void fixNoTouch(GameObject* object, CCArray* objects);
};

class $modify(NTPSFEditorUI, EditorUI) {
    $registerHooks(NoTouchPasteStateFix)

    void onPasteState(cocos2d::CCObject* sender);
};