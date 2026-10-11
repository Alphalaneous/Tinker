#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $module(NoTouchPasteStateFix) {
    bool onToggled(bool state);

    void fixNoTouch(GameObject* object, cocos2d::CCArray* objects);
};

class $modify(NTPSFEditorUI, EditorUI) {
    $registerHooks(NoTouchPasteStateFix)

    void onPasteState(cocos2d::CCObject* sender);
};