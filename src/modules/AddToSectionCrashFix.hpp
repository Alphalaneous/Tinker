#pragma once

#include "module/Module.hpp"
#include <Geode/modify/GJBaseGameLayer.hpp>

class $editorModule(AddToSectionCrashFix) {
    bool onToggled(bool state) override;
};

class $modify(ATSCFGJBaseGameLayer, GJBaseGameLayer) {
    $registerEditorHooks(AddToSectionCrashFix)

	void addToSection(GameObject* object);
};