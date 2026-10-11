#pragma once

#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>
#include <Geode/cocos/touch_dispatcher/CCTouch.h>

namespace tinker::ui {

class ToolbarTouch : public cocos2d::CCLayer {
public:
    static ToolbarTouch* create();
protected:
    bool init() override;
    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void registerWithTouchDispatcher() override;
};

}