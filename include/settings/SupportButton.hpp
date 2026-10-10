#pragma once

#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>
#include <Geode/cocos/sprite_nodes/CCSprite.h>
#include <Geode/cocos/touch_dispatcher/CCTouch.h>

using namespace geode::prelude;

namespace tinker::ui {

class SupportButton : public CCLayer {
public:
    static SupportButton* create(float width);
protected:
    bool init(float width);

    bool ccTouchBegan(CCTouch* touch, CCEvent* event);
    void ccTouchMoved(CCTouch* touch, CCEvent* event);
    void ccTouchEnded(CCTouch* touch, CCEvent* event);
    void ccTouchCancelled(CCTouch* touch, CCEvent* event);
    void registerWithTouchDispatcher();

    CCSprite* m_banner;
};

}