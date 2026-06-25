#pragma once

#include "module/Module.hpp"
#include <Geode/modify/CreateMenuItem.hpp>

class $editorModule(CenteredObjectButtons) {};

class $modify(COBCreateMenuItem, CreateMenuItem) {
    $registerEditorHooks(CenteredObjectButtons)

    static CreateMenuItem* create(cocos2d::CCNode* normal, cocos2d::CCNode* selected, cocos2d::CCObject* target, cocos2d::SEL_MenuHandler selector);
};