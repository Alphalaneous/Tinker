#pragma once

#include "module/Module.hpp"
#include <Geode/modify/LevelSettingsLayer.hpp>

class $editorModule(ObjectSummary) {};

class $modify(OSLevelSettingsLayer, LevelSettingsLayer) {
    $registerEditorHooks(ObjectSummary)

	bool init(LevelSettingsObject* object, LevelEditorLayer* layer);
};
