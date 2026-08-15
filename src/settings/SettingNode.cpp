#include "settings/SettingNode.hpp"

namespace tinker::settings {

bool SettingNodeBase::init(float width, std::shared_ptr<SettingV3> setting) {
    if (!CCLayerColor::initWithColor({0, 0, 0, 50})) return false;
    m_setting = setting;

    setContentSize({width, 30});
    ignoreAnchorPointForPosition(false);
    setAnchorPoint({0.5f, 0.5f});

    m_titleContainer = CCNode::create();
    m_titleContainer->setAnchorPoint({0.f, 0.5f});
    m_titleContainer->setContentSize({width / 2.f, 15.f});
    m_titleContainer->setPosition({10, getContentHeight() / 2.f});

    m_titleContainer->setLayout(SimpleRowLayout::create()
        ->setMinRelativeScale(0.01f)
        ->setGap(5.f)
        ->setMainAxisScaling(AxisScaling::Scale)
        ->setMainAxisAlignment(MainAxisAlignment::Start)
    );

    m_title = CCLabelBMFont::create(setting->getDisplayName().c_str(), "bigFont.fnt");
    m_title->limitLabelWidth(width / 2.f - 20.f, 1.f, 0.01f);
    m_titleContainer->addChild(m_title);

    m_infoButton = geode::Button::createWithSpriteFrameName("GJ_infoIcon_001.png", [this] (auto sender) {
        if (m_setting) {
            auto description = m_setting->getDescription();
            if (!description) return;

            MDPopup::create(m_setting->getDisplayName(), description.value(), "OK", nullptr, nullptr)->show();
        }
    });
    m_infoButton->setScale(0.5f);

    if (!setting->getDescription().has_value()) {
        m_infoButton->setVisible(false);
    }

    m_titleContainer->addChild(m_infoButton);

    m_titleContainer->updateLayout();

    addChild(m_titleContainer);

    return true;
}

bool SettingNodeBase::settingWasChanged() {
    return false;
}

void SettingNodeBase::updateState() {
    
}

void SettingNodeBase::commit() {
    
}

void SettingNodeBase::resetToDefault() {
    
}

std::shared_ptr<SettingV3> SettingNodeBase::getSetting() {
    return m_setting;
}

}
