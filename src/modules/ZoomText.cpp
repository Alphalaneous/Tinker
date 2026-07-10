#include "ZoomText.hpp"
#include "../../include/UIScaling.hpp"

void ZoomText::onEditor() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    m_zoomLabel = CCLabelBMFont::create("", "bigFont.fnt");
    m_zoomLabel->setScale(0.5f);
    m_zoomLabel->setPosition(winSize.width / 2.f, winSize.height - 60.f);
    m_zoomLabel->setID("zoom-text"_spr);
    m_zoomLabel->setOpacity(0);
    m_zoomLabel->setZOrder(99999);
    m_editorUI->addChild(m_zoomLabel);

    addEventListener(tinker::api::ui_scaling::UIScaleUpdated(), [this] (float scale, bool scaleToolbars, bool topAlign) {
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        m_zoomLabel->setPosition(winSize.width / 2.f, winSize.height - 60.f * scale);
        m_zoomLabel->setScale(0.5f * scale);
    });
}

void ZoomText::showZoomText() {
    if (!m_zoomLabel) return;

    m_zoomLabel->setString(fmt::format("Zoom: {}x", numToString(m_editorLayer->m_objectLayer->getScale(), 2)).c_str());
    m_zoomLabel->setOpacity(255);
    m_zoomLabel->stopAllActions();
    m_zoomLabel->runAction(CCSequence::create(
        CCDelayTime::create(0.5f),
        CCFadeOut::create(0.5f),
        nullptr
    ));
}

void ZTEditorUI::updateZoom(float zoom) {
    EditorUI::updateZoom(zoom);
    ZoomText::get()->showZoomText();
}