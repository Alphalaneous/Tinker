#include "ImprovedLinkControls.hpp"
#include "modules/UIScaling.hpp"

void ImprovedLinkControls::onEditor() {
    #ifndef GEODE_IS_MOBILE
    auto zoomMenu = m_editorUI->getChildByID("zoom-menu");
    auto linkMenu = m_editorUI->getChildByID("link-menu");
    linkMenu->setContentSize({ 125.f, zoomMenu->getContentHeight() + 29.f });
    static_cast<AxisLayout*>(linkMenu->getLayout())->setGap(3.f);
    linkMenu->updateLayout();

    addEventListener(UIScaleUpdated(), [linkMenu, zoomMenu] (float scale, bool scaleToolbars, bool fullReload) {
        linkMenu->setScale(scale * 0.8f);  
        linkMenu->setPosition({zoomMenu->getPositionX() + zoomMenu->getScaledContentWidth() / 2.f + linkMenu->getScaledContentWidth() / 2.f + 5.f * scale, zoomMenu->getPositionY()});
    });

    linkMenu->addOnEnterCallback([linkMenu, zoomMenu] {
        float scale = 1.f;
        if (UIScaling::isEnabled()) {
            scale = UIScaling::get()->m_scale;
        }
        linkMenu->setScale(scale * 0.8f);
        linkMenu->setPosition({zoomMenu->getPositionX() + zoomMenu->getScaledContentWidth() / 2.f + linkMenu->getScaledContentWidth() / 2.f + 5.f * scale, zoomMenu->getPositionY()});
    });

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
