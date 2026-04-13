#pragma once

#include "../Module.hpp"
#include <Geode/modify/GJTransformControl.hpp>
#include <Geode/modify/GJScaleControl.hpp>

class $globalModule(TransformFix) {
    bool onToggled(bool state) override;
};

class $modify(TFGJTransformControl, GJTransformControl) {
    $registerGlobalHooks(TransformFix, true);

    struct Fields {
        CCMenu* m_menu;
        bool m_touchInMenu = false;
    };

    void scaleButtons(float scale);

    CCMenu* getMenu();
    bool init();

    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);

    static void _onModify(auto& self) {
        (void) self.setHookPriorityBeforePre("GJTransformControl::ccTouchBegan", "razoom.improved_transform_controls");
        (void) self.setHookPriorityBeforePre("GJTransformControl::ccTouchMoved", "razoom.improved_transform_controls");
        (void) self.setHookPriorityBeforePre("GJTransformControl::ccTouchEnded", "razoom.improved_transform_controls");
        (void) self.setHookPriorityBeforePre("GJTransformControl::ccTouchCancelled", "razoom.improved_transform_controls");
    }
};

class $modify(TFGJScaleControl, GJScaleControl) {
    $registerGlobalHooks(TransformFix);

    struct Fields {
        CCMenu* m_menu;
        bool m_touchInMenu = false;
    };

    CCMenu* getMenu();

    bool init();

    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
};