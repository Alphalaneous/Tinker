#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EffectGameObject.hpp>

class $module(OldColorTriggers) {
    static std::unordered_map<int, std::string> s_textureMap;
};

class $modify(OCTEffectGameObject, EffectGameObject) {
    $registerHooks(OldColorTriggers)
	void customSetup();
};