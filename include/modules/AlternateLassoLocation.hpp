#pragma once

#include "module/Module.hpp"

class $module(AlternateLassoLocation) {
    bool onToggled(bool state);
    void onEditor();
    void onLasso(cocos2d::CCObject* sender);

    geode::Ref<CCMenuItemToggler> m_originalToggler = nullptr;
    geode::Ref<CCMenuItemToggler> m_newToggler = nullptr;

};