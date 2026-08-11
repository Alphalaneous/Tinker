#include "modules/ImprovedLinkControls.hpp"

void ILCEditorUI::onGroupSticky(CCObject* sender) {
    if (m_linkControlsDisabled) return;
    EditorUI::onGroupSticky(sender);
}

void ILCEditorUI::onUngroupSticky(CCObject* sender) {
    if (m_linkControlsDisabled) return;
    EditorUI::onUngroupSticky(sender);
}
