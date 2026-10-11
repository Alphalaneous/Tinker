#pragma once

#include <Geode/cocos/cocoa/CCObject.h>
#include <Geode/utils/function.hpp>

class DestroyListener : public cocos2d::CCObject {
public:
    virtual ~DestroyListener();

    static DestroyListener* create(geode::Function<void()> destroyCallback);
protected:
    geode::Function<void()> m_destroyCallback = nullptr;
};