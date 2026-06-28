#include "EditorTouchFix.hpp"

bool EditorTouchFix::onToggled(bool state) {
    return true;
}

void ETFEditorUI::onPlaytest(cocos2d::CCObject* sender) {
    m_isDraggingCamera = false;
    m_swipeActive = false;
    m_continueSwipe = false;

    EditorUI::onPlaytest(sender);
}
