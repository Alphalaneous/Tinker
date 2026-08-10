#include "modules/ZoomText.hpp"

bool ZoomText::onToggled(bool state) {
    if (state) {
        onEditor();
    }
    else {
        m_zoomLabel->removeFromParent();
        m_zoomLabel = nullptr;
        removeEventListener("ui-scale");
        removeEventListener("editor-zoom");
    }

    return true;
}

void ZoomText::onEditor() {
    auto winSize = CCDirector::get()->getWinSize();

    m_zoomLabel = CCLabelBMFont::create("", "bigFont.fnt");
    m_zoomLabel->setScale(0.5f);
    m_zoomLabel->setPosition(winSize.width / 2.f, winSize.height - 60.f);
    m_zoomLabel->setID("zoom-text"_spr);
    m_zoomLabel->setOpacity(0);
    m_zoomLabel->setZOrder(99999);
    getEditor()->addChild(m_zoomLabel);

    addEventListener("ui-scale", UIScaleUpdated(), [this] (float scale, bool scaleToolbars, bool fullReload) {
        auto winSize = CCDirector::get()->getWinSize();
        m_zoomLabel->setPosition(winSize.width / 2.f, winSize.height - 60.f * scale);
        m_zoomLabel->setScale(0.5f * scale);
    });

    addEventListener("editor-zoom", EditorZoomEvent(), [this] (float zoom) {
        if (!m_zoomLabel) return;

        m_zoomLabel->setString(fmt::format("Zoom: {}x", numToString(getEditorLayer()->m_objectLayer->getScale(), 2)).c_str());
        m_zoomLabel->setOpacity(255);
        m_zoomLabel->stopAllActions();
        m_zoomLabel->runAction(CCSequence::create(
            CCDelayTime::create(0.5f),
            CCFadeOut::create(0.5f),
            nullptr
        ));
    });
}