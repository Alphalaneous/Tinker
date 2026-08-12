#pragma once

#include <Geode/Geode.hpp>

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