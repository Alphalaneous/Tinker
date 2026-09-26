#include "nodes/ToolbarTouch.hpp"
#include "utils/Utils.hpp"

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
    return touch->getLocation().y <= utils::getToolbarHeight(true);
}

void ToolbarTouch::registerWithTouchDispatcher() {
    CCTouchDispatcher::get()->addTargetedDelegate(this, 0, true);
}

}