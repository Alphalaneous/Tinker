#pragma once

#include "module/Module.hpp"
#include <Geode/modify/LevelSettingsLayer.hpp>

class $module(ImprovedArtPicker) {
    bool onToggled(bool state);
};

class $modify(IAPLevelSettingsLayer, LevelSettingsLayer) {
    $registerHooks(ImprovedArtPicker)

    void onBGArt(cocos2d::CCObject* sender);
    void onGArt(cocos2d::CCObject* sender);
    void onFGArt(cocos2d::CCObject* sender);
};