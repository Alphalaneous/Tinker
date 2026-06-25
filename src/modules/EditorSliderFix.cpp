#include "EditorSliderFix.hpp"

void EditorSliderFix::onEditor() {
    addEventListener(ObjectChangeEvent(), [this] (float lastObjectX) {
        m_lastObjectX = lastObjectX;
        m_editorUI->updateSlider();
    });
}

bool EditorSliderFix::onToggled(bool state) {
    return true;
}

void ESFEditorUI::sliderChanged(cocos2d::CCObject* sender) {
    auto module = EditorSliderFix::get();

    float maxX = (module->m_lastObjectX + 300) * m_editorLayer->m_objectLayer->getScale();
    float x = -(maxX * m_positionSlider->getThumb()->getValue()) + 100;
    float y = m_editorLayer->m_objectLayer->getPositionY();
    m_editorLayer->m_objectLayer->setPosition({x, y});
    constrainGameLayerPosition(-100, -100);
}

void ESFEditorUI::updateSlider() {
    auto module = EditorSliderFix::get();

    float maxX = (module->m_lastObjectX + 300) * m_editorLayer->m_objectLayer->getScale();
    float x = -m_editorLayer->m_objectLayer->getPositionX() + 100;
    float pos = std::clamp(x / maxX, 0.0f, 1.0f);
    m_positionSlider->setValue(pos);
}