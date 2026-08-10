#pragma once

#include "module/Module.hpp"
#include <Geode/modify/LevelEditorLayer.hpp>

class $module(FixHeadHitting) {
    bool onToggled(bool state);
};

class $modify(FHHLevelEditorLayer, LevelEditorLayer) {
    $registerHooks(FixHeadHitting)

    void onPlaytest();
};