#pragma once

#include <Geode/cocos/cocoa/CCObject.h>
#include <Geode/utils/function.hpp>

using namespace geode::prelude;

namespace tinker::utils {

class HijackCallback : public CCObject {
    public:
    using Hijack = geode::Function<void(std::function<void(CCObject* sender)> orig, CCObject* sender)>;

    static HijackCallback* create(Hijack method, SEL_MenuHandler originalSelector);
    bool init(Hijack method, SEL_MenuHandler originalSelector);
    void callback(CCObject* sender);

    Hijack m_method;
    SEL_MenuHandler m_selector;
};

void hijackButton(CCMenuItem* btn, HijackCallback::Hijack method);

}