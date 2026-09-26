#pragma once


#include <Geode/Geode.hpp>

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