#pragma once

#include "module/Module.hpp"
#include <Geode/modify/LevelEditorLayer.hpp>

class $module(SelectionFix) {
    bool onToggled(bool state);
};

class $modify(SFLevelEditorLayer, LevelEditorLayer) {
    $registerHooks(SelectionFix)

    GameObject* objectAtPosition(cocos2d::CCPoint position);
    cocos2d::CCArray* objectsAtPosition(cocos2d::CCPoint position);
};

