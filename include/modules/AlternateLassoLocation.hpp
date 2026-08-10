#pragma once

#include "module/Module.hpp"

class $module(AlternateLassoLocation) {
    void onEditor();
    void onLasso(CCObject* sender);

    Ref<CCMenuItemToggler> m_originalToggler;
};