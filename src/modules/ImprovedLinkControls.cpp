#include "ImprovedLinkControls.hpp"
#include "UIScaling.hpp"

void ImprovedLinkControls::onEditor() {
    #ifndef GEODE_IS_MOBILE
    auto zoomMenu = m_editorUI->getChildByID("zoom-menu");
    auto linkMenu = m_editorUI->getChildByID("link-menu");
    linkMenu->setContentSize({ 125.f, zoomMenu->getContentHeight() + 29.f });
    linkMenu->setPosition({linkMenu->getPositionX() - 5, zoomMenu->getPositionY()});
    static_cast<AxisLayout*>(linkMenu->getLayout())->setGap(0.f);
    linkMenu->updateLayout();
    
    // for BetterEdit
    if (!UIScaling::isEnabled()) {
        linkMenu->runAction(CallFuncExt::create([linkMenu] {
            linkMenu->setScale(0.8f * linkMenu->getScale());
        }));
    }
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
