#include "nodes/ColorVisualButton.hpp"
#include "nodes/ColorChannelSprite.hpp"

namespace tinker::ui {

ColorVisualButton* ColorVisualButton::create(EditorUI* editorUI) {
    auto ret = new ColorVisualButton();
    if (ret->init(editorUI)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
};

bool ColorVisualButton::init(EditorUI* editorUI) {
    m_colorChannelSprite = tinker::ui::ColorChannelSprite::create(0);
    m_colorChannelSprite->setLive(true);
    
    if (!CCMenuItemSpriteExtra::init(
        m_colorChannelSprite, 
        nullptr, 
        this, 
        menu_selector(ColorVisualButton::openColorPicker)
    )) return false;

    m_editorUI = editorUI;

    setVisible(false);
    return true;
}

void ColorVisualButton::setColorData(int id) {
    setVisible(id != -1);
    if (id == -1) return;

    m_action = LevelEditorLayer::get()->m_levelSettings->m_effectManager->getColorAction(id);
    m_colorChannelSprite->setColorID(id);
}

void ColorVisualButton::openColorPicker(CCObject* obj) {
    auto popup = ColorSelectPopup::create(m_action);
    popup->show();
    popup->setZOrder(CCScene::get()->getHighestChildZ() + 1);
}

}