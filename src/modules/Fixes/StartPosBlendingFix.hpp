#pragma once

#include "module/Module.hpp"
#include <Geode/modify/LevelEditorLayer.hpp>

class $editorModule(StartPosBlendingFix) {
    bool onToggled(bool state) override;
};

class $modify(SPBFLevelEditorLayer, LevelEditorLayer) {
    $registerEditorHooks(StartPosBlendingFix)

    void onPlaytest();
};