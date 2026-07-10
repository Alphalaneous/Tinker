#include "HideUI.hpp"

void HideUI::onEditor() {
    auto undoMenu = m_editorUI->getChildByID("undo-menu");
    if (!undoMenu) return;

    auto showEye = CCSprite::create("show-eye.png"_spr);
    showEye->setOpacity(100);
    showEye->setScale(0.75f);

    auto hideEye = CCSprite::create("hide-eye.png"_spr);
    hideEye->setOpacity(50);
    hideEye->setScale(0.75f);

    auto toggler = CCMenuItemExt::createToggler(hideEye, showEye, [this] (auto sender) {
        m_editorUI->showUI(sender->isToggled());
    });
    toggler->m_notClickable = true;

    undoMenu->addChild(toggler);

    undoMenu->updateLayout();

    undoMenu->addOnEnterCallback([this, undoMenu] {
        m_oldBEButton = undoMenu->getChildByID("hjfod.betteredit/hide-ui-toggle");
        if (m_oldBEButton) {
            m_oldBEButton->removeFromParent();
        }
    });

    addEventListener(ShowUIEvent(), [this, toggler] (bool show) {
        toggler->toggle(!show);
        toggler->setVisible(m_editorLayer->m_playbackMode != PlaybackMode::Playing);
    });
}