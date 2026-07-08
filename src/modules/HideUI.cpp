#include "HideUI.hpp"

// todo make sprites
void HideUI::onEditor() {
    auto undoMenu = m_editorUI->getChildByID("undo-menu");
    if (!undoMenu) return;
    
    auto toggler = CCMenuItemExt::createTogglerWithFrameName("GJ_likeBtn_001.png", "GJ_dislikeBtn_001.png", 0.6f, [this] (auto sender) {
        m_editorUI->showUI(sender->isToggled());
    });
    toggler->m_notClickable = true;

    undoMenu->addChild(toggler);
    undoMenu->updateLayout();

    addEventListener(ShowUIEvent(), [this, toggler] (bool show) {
        toggler->toggle(!show);
        toggler->setVisible(m_editorLayer->m_playbackMode != PlaybackMode::Playing);
    });
}