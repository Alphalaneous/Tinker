#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorOptionsLayer.hpp>

class $editorModule(ButtonRowsBypass) {
    bool onToggled(bool state) override;
};

class $modify(BRBEditorOptionsLayer, EditorOptionsLayer) {
    $registerEditorHooks(ButtonRowsBypass)

    void onButtonRows(cocos2d::CCObject* sender);
    void onButtonsPerRow(CCObject* sender);
};