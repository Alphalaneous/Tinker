#include "modules/ImprovedArtPicker.hpp"
#include "nodes/SelectArtLayer.hpp"

bool ImprovedArtPicker::onToggled(bool state) {
    return true;
}

void IAPLevelSettingsLayer::onBGArt(cocos2d::CCObject* sender) {
    tinker::ui::SelectArtLayer::create(m_editorLayer, this, SelectArtType::Background)->show();
}

void IAPLevelSettingsLayer::onGArt(cocos2d::CCObject* sender) {
    tinker::ui::SelectArtLayer::create(m_editorLayer, this, SelectArtType::Ground)->show();
}

void IAPLevelSettingsLayer::onFGArt(cocos2d::CCObject* sender) {
    tinker::ui::SelectArtLayer::create(m_editorLayer, this, SelectArtType::Middleground)->show();
}