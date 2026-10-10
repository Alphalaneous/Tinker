#pragma once

#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>
#include <Geode/cocos/touch_dispatcher/CCTouch.h>

using namespace geode::prelude;

namespace tinker::ui {

class ToolbarTouch : public CCLayer {
public:
    static ToolbarTouch* create();
protected:
    bool init() override;
    bool ccTouchBegan(CCTouch* touch, CCEvent* event) override;
    void registerWithTouchDispatcher() override;
};

}