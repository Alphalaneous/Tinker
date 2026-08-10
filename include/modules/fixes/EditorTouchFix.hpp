#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $module(EditorTouchFix) {
    bool onToggled(bool state);
};

class $modify(ETFEditorUI, EditorUI) {
    $registerHooks(EditorTouchFix)

    struct Fields {
        Ref<CCTouch> m_lastTouch;
    };

    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void onPlaytest(cocos2d::CCObject* sender);
};