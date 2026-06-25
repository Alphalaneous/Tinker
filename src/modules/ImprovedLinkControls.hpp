#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $editorModule(ImprovedLinkControls) {
    void onEditor() override;
};

class $modify(ILCEditorUI, EditorUI) {
    $registerEditorHooks(ImprovedLinkControls);

    void onGroupSticky(cocos2d::CCObject* sender);
    void onUngroupSticky(cocos2d::CCObject* sender);
};