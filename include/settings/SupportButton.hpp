#pragma once

#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>
#include <Geode/cocos/sprite_nodes/CCSprite.h>
#include <Geode/cocos/touch_dispatcher/CCTouch.h>

namespace tinker::ui {

class SupportButton : public cocos2d::CCLayer {
public:
    static SupportButton* create(float width);
protected:
    bool init(float width);

    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void registerWithTouchDispatcher();

    cocos2d::CCSprite* m_banner = nullptr;
};

}