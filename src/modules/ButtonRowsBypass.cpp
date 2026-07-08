#include "ButtonRowsBypass.hpp"

bool ButtonRowsBypass::onToggled(bool state) {
    return true;
}

void BRBEditorOptionsLayer::onButtonRows(cocos2d::CCObject* sender) {
    m_buttonRows = std::clamp(m_buttonRows + (sender->getTag() ? 1 : -1), 2, 24);
    m_buttonRowsLabel->setString(numToString(m_buttonRows).c_str());
}

void BRBEditorOptionsLayer::onButtonsPerRow(CCObject* sender) {
    m_buttonsPerRow = std::clamp(m_buttonsPerRow + (sender->getTag() ? 1 : -1), 6, 128);
    m_buttonsPerRowLabel->setString(numToString(m_buttonsPerRow).c_str());
}