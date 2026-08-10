#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorOptionsLayer.hpp>

class $module(ButtonRowsBypass) {
    bool onToggled(bool state);
};

class $modify(BRBEditorOptionsLayer, EditorOptionsLayer) {
    $registerHooks(ButtonRowsBypass)

    void onButtonRows(cocos2d::CCObject* sender);
    void onButtonsPerRow(CCObject* sender);
};