#include "modules/fixes/EditorSliderFix.hpp"

void EditorSliderFix::onEditor() {
    addEventListener(ObjectChangeEvent(), [this] (float lastObjectX) {
        m_lastObjectX = lastObjectX;
        getEditor()->updateSlider();
    });
}

bool EditorSliderFix::onToggled(bool state) {
    return true;
}

void ESFEditorUI::sliderChanged(cocos2d::CCObject* sender) {
    auto module = EditorSliderFix::get();
    auto value = m_positionSlider->getThumb()->getValue();
    
    if (module->m_lastSliderValue != value) {
        float maxX = (module->m_lastObjectX + 800.f) * m_editorLayer->m_objectLayer->getScale();
        float x = -(maxX * value) + 100.f;
        float y = m_editorLayer->m_objectLayer->getPositionY();
        m_editorLayer->m_objectLayer->setPosition({x, y});
    }

    module->m_lastSliderValue = value;

    constrainGameLayerPosition();
}

void ESFEditorUI::updateSlider() {
    auto module = EditorSliderFix::get();

    float maxX = (module->m_lastObjectX + 800.f) * m_editorLayer->m_objectLayer->getScale();
    float x = -m_editorLayer->m_objectLayer->getPositionX() + 100.f;
    float pos = std::clamp(x / maxX, 0.f, 1.f);
    
    module->m_lastSliderValue = pos;
    m_positionSlider->setValue(pos);
}