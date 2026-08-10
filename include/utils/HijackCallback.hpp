#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class HijackCallback : public CCObject {
    public:
    using Hijack = std::function<void(std::function<void(CCObject* sender)> orig, CCObject* sender)>;

    static HijackCallback* create(Hijack method, SEL_MenuHandler originalSelector);
    bool init(Hijack method, SEL_MenuHandler originalSelector);
    void callback(CCObject* sender);

    Hijack m_method;
    SEL_MenuHandler m_selector;
};