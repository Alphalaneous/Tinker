#include "ImprovedLinkControls.hpp"

void ImprovedLinkControls::onEditor() {
    #ifndef GEODE_IS_MOBILE
    auto zoomMenu = m_editorUI->getChildByID("zoom-menu");
    auto linkMenu = m_editorUI->getChildByID("link-menu");
    linkMenu->setContentSize({ 125.f, zoomMenu->getContentHeight() + 29.f });
    linkMenu->setPosition({linkMenu->getPositionX() - 5.f, zoomMenu->getPositionY()});
    static_cast<AxisLayout*>(linkMenu->getLayout())->setGap(3.f);
    linkMenu->updateLayout();
    linkMenu->setScale(0.8f);
    #endif
}

void ILCEditorUI::onGroupSticky(CCObject* sender) {
    if (m_linkControlsDisabled) return;
    EditorUI::onGroupSticky(sender);
}

void ILCEditorUI::onUngroupSticky(CCObject* sender) {
    if (m_linkControlsDisabled) return;
    EditorUI::onUngroupSticky(sender);
}
