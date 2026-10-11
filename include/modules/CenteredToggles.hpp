#pragma once

#include "module/Module.hpp"

class $module(CenteredToggles) {
    bool onToggled(bool state);
    void onEditor();

    void setOffset(cocos2d::CCNode* node, float offset);
};