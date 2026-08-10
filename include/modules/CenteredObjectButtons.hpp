#pragma once

#include "module/Module.hpp"
#include <Geode/modify/CreateMenuItem.hpp>

class $module(CenteredObjectButtons) {};

class $modify(COBCreateMenuItem, CreateMenuItem) {
    $registerHooks(CenteredObjectButtons)

    static CreateMenuItem* create(cocos2d::CCNode* normal, cocos2d::CCNode* selected, cocos2d::CCObject* target, cocos2d::SEL_MenuHandler selector);
};