#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EffectGameObject.hpp>

class $module(FixToggleTriggerVisibility) {};

class $modify(FTTVEffectGameObject, EffectGameObject) {
    $registerHooks(FixToggleTriggerVisibility)

    void setOpacity(unsigned char opacity);
};