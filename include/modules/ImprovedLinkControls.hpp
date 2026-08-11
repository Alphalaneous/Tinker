#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $module(ImprovedLinkControls) {
};

class $modify(ILCEditorUI, EditorUI) {
    $registerHooks(ImprovedLinkControls);

    void onGroupSticky(cocos2d::CCObject* sender);
    void onUngroupSticky(cocos2d::CCObject* sender);
};