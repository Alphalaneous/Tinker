#pragma once

#include "module/Module.hpp"
#include <Geode/modify/GameSettingsLayer.hpp>

class $module(ButtonRowsBypass) {
    bool onToggled(bool state);
};

class $modify(BRBGameSettingsLayer, GameSettingsLayer) {
    $registerHooks(ButtonRowsBypass)

    void onButtonRows(cocos2d::CCObject* sender);
    void onButtonsPerRow(cocos2d::CCObject* sender);
};