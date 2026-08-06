#pragma once

#include "module/Module.hpp"
#include <Geode/modify/LevelEditorLayer.hpp>

class $editorModule(FixHeadHitting) {
    bool onToggled(bool state) override;
};

class $modify(FHHLevelEditorLayer, LevelEditorLayer) {
    $registerEditorHooks(FixHeadHitting)

    void onPlaytest();
};