#pragma once

#include "module/Module.hpp"
#include <Geode/modify/LevelEditorLayer.hpp>

class $module(ShakeTriggerFix) {
    bool onToggled(bool state);
};

class $modify(STFLevelEditorLayer, LevelEditorLayer) {
    $registerHooks(ShakeTriggerFix)

    void addSpecial(GameObject* object);
};

