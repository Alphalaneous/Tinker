#pragma once

#include "module/Module.hpp"
#include <Geode/modify/LevelSettingsLayer.hpp>

class $module(ObjectSummary) {
    bool onToggled(bool state);
};

class $modify(OSLevelSettingsLayer, LevelSettingsLayer) {
    $registerHooks(ObjectSummary)

	bool init(LevelSettingsObject* object, LevelEditorLayer* layer);
};
