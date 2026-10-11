#include "utils/HijackCallback.hpp"

using namespace geode::prelude;

namespace tinker::utils {

HijackCallback* HijackCallback::create(Hijack method, SEL_MenuHandler originalSelector) {
    auto ret = new HijackCallback();
    if (ret->init(std::move(method), originalSelector)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool HijackCallback::init(Hijack method, SEL_MenuHandler originalSelector) {
    m_method = std::move(method);
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

void hijackButton(CCMenuItem* btn, HijackCallback::Hijack method) {
    if (btn->getUserObject("hijack"_spr)) return;

    auto hijack = HijackCallback::create(std::move(method), btn->m_pfnSelector);
    btn->setUserObject("hijack"_spr, hijack);
    btn->m_pfnSelector = menu_selector(HijackCallback::callback);
}

}