#include "utils/HijackCallback.hpp"

HijackCallback* HijackCallback::create(Hijack method, SEL_MenuHandler originalSelector) {
    auto ret = new HijackCallback();
    if (ret->init(method, originalSelector)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool HijackCallback::init(Hijack method, SEL_MenuHandler originalSelector) {
    m_method = method;
    m_selector = originalSelector;
    return true;
}

void HijackCallback::callback(CCObject* sender) {
    auto btn = static_cast<CCMenuItem*>(sender);
    auto hijack = static_cast<HijackCallback*>(btn->getUserObject("hijack"_spr));
    
    if (hijack->m_method) hijack->m_method([btn, hijack] (CCObject* sender) {
        (btn->m_pListener->*hijack->m_selector)(sender);
    }, sender);
}