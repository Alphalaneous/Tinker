#include "modules/fixes/FixHeadHitting.hpp"

bool FixHeadHitting::onToggled(bool state) {
    return true;
}

void FHHLevelEditorLayer::onPlaytest() {
    m_player1->m_lastFlipTime = 0;
    m_player2->m_lastFlipTime = 0;

    LevelEditorLayer::onPlaytest();
}
