#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $module(EditorTouchFix) {
    bool onToggled(bool state);
};

class $modify(ETFEditorUI, EditorUI) {
    $registerHooks(EditorTouchFix)

    struct Fields {
        geode::Ref<cocos2d::CCTouch> m_lastTouch = nullptr;
    };

    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void onPlaytest(cocos2d::CCObject* sender);
};