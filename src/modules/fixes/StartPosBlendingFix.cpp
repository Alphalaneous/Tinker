#include "modules/fixes/StartPosBlendingFix.hpp"

bool StartPosBlendingFix::onToggled(bool state) {
    return true;
}

void SPBFLevelEditorLayer::onPlaytest() {
    LevelEditorLayer::onPlaytest();
    updateBlendValues();
}