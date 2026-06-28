#include "EditorTouchFix.hpp"

bool EditorTouchFix::onToggled(bool state) {
    return true;
}

bool ETFEditorUI::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    auto ret = EditorUI::ccTouchBegan(touch, event);
    m_fields->m_lastTouch = touch;
    return ret;
}

void ETFEditorUI::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    EditorUI::ccTouchMoved(touch, event);
    m_fields->m_lastTouch = touch;
}

void ETFEditorUI::onPlaytest(cocos2d::CCObject* sender) {
    ccTouchCancelled(m_fields->m_lastTouch, nullptr);
    EditorUI::onPlaytest(sender);
}
