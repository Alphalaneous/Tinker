#pragma once

#include <Geode/cocos/cocoa/CCObject.h>
#include <Geode/utils/function.hpp>

namespace tinker::utils {

class HijackCallback : public cocos2d::CCObject {
    public:
    using Hijack = geode::Function<void(std::function<void(cocos2d::CCObject* sender)> orig, cocos2d::CCObject* sender)>;

    static HijackCallback* create(Hijack method, cocos2d::SEL_MenuHandler originalSelector);
    bool init(Hijack method, cocos2d::SEL_MenuHandler originalSelector);
    void callback(cocos2d::CCObject* sender);

    Hijack m_method = nullptr;
    cocos2d::SEL_MenuHandler m_selector = nullptr;
};

void hijackButton(cocos2d::CCMenuItem* btn, HijackCallback::Hijack method);

}