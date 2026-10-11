#include "modules/fixes/StartPosBlendingFix.hpp"

using namespace geode::prelude;

bool StartPosBlendingFix::onToggled(bool state) {
    return true;
}

void SPBFLevelEditorLayer::onPlaytest() {
    LevelEditorLayer::onPlaytest();
    updateBlendValues();
}