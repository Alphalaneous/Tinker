#pragma once

#include "module/Module.hpp"
#include <Geode/modify/LevelEditorLayer.hpp>

class $module(StartPosBlendingFix) {
    bool onToggled(bool state);
};

class $modify(SPBFLevelEditorLayer, LevelEditorLayer) {
    $registerHooks(StartPosBlendingFix)

    void onPlaytest();
};