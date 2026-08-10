#pragma once

#include "module/Module.hpp"
#include <Geode/modify/GJBaseGameLayer.hpp>

class $module(AddToSectionCrashFix) {
    bool onToggled(bool state);
};

class $modify(ATSCFGJBaseGameLayer, GJBaseGameLayer) {
    $registerHooks(AddToSectionCrashFix)

	void addToSection(GameObject* object);
};