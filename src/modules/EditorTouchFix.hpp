#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $editorModule(EditorTouchFix) {
    bool onToggled(bool state) override;
};

class $modify(ETFEditorUI, EditorUI) {
    $registerEditorHooks(EditorTouchFix)

    void onPlaytest(cocos2d::CCObject* sender);
};