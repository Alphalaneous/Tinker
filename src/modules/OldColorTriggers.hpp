#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EffectGameObject.hpp>

class $editorModule(OldColorTriggers) {
    static std::unordered_map<int, std::string> s_textureMap;
};

class $modify(OCTEffectGameObject, EffectGameObject) {
    $registerEditorHooks(OldColorTriggers)
	void customSetup();
};