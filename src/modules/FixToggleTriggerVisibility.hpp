#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EffectGameObject.hpp>

class $editorModule(FixToggleTriggerVisibility) {};

class $modify(FTTVEffectGameObject, EffectGameObject) {
    $registerEditorHooks(FixToggleTriggerVisibility)

    void setOpacity(unsigned char opacity);
};