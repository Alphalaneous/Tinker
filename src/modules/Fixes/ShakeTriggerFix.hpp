#pragma once

#include "module/Module.hpp"
#include <Geode/modify/LevelEditorLayer.hpp>

class $editorModule(ShakeTriggerFix) {
    bool onToggled(bool state);
};

class $modify(STFLevelEditorLayer, LevelEditorLayer) {
    $registerEditorHooks(ShakeTriggerFix)

    void addSpecial(GameObject* object);
};

