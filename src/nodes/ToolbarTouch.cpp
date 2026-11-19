#include "nodes/ToolbarTouch.hpp"
#include "utils/UI.hpp"

using namespace geode::prelude;

namespace tinker::ui {

ToolbarTouch* ToolbarTouch::create() {
    auto ret = new ToolbarTouch();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool ToolbarTouch::init() {
    if (!CCLayer::init()) return false;

    setTouchEnabled(true);

    return true;
}

bool ToolbarTouch::ccTouchBegan(CCTouch* touch, CCEvent* event) {
    //return touch->getLocation().y <= tinker::utils::ui::getToolbarHeight(true);
    return false;
}

void ToolbarTouch::registerWithTouchDispatcher() {
    CCTouchDispatcher::get()->addTargetedDelegate(this, 0, true);
}

}