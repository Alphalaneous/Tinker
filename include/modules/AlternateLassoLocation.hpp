#pragma once

#include "module/Module.hpp"

class $module(AlternateLassoLocation) {
    bool onToggled(bool state);
    void onEditor();
    void onLasso(CCObject* sender);

    Ref<CCMenuItemToggler> m_originalToggler;
    Ref<CCMenuItemToggler> m_newToggler;

};