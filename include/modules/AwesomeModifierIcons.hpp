#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EffectGameObject.hpp>

class $module(AwesomeModifierIcons) {
    static std::unordered_map<int, std::string> s_textureMap;
};

class $modify(AMIEffectGameObject, EffectGameObject) {
    $registerHooks(AwesomeModifierIcons)

	void customSetup();
    void updateLetters();
};