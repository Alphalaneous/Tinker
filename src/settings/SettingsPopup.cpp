#include "settings/SettingsPopup.hpp"
#include "alphalaneous.alphas-ui-pack/include/nodes/scroll/AdvancedScrollLayer.hpp"
#include "settings/SettingNodeRegistry.hpp"
#include "settings/SettingNode.hpp"

SettingsPopup* SettingsPopup::create() {
    auto ret = new SettingsPopup();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool SettingsPopup::init() {
    if (!Popup::init({450, 280}, "simple-popup-square.png"_spr)) return false;

    m_scrollLayer = alpha::ui::AdvancedScrollLayer::create({300.f, m_size.height - 40.f});
    m_scrollLayer->setAnchorPoint({1.f, 1.f});
    m_scrollLayer->setPosition(m_size - CCSize{10.f, 5.f});
    m_scrollLayer->getContentLayer()->setLayout(SimpleColumnLayout::create()
        ->setMainAxisDirection(AxisDirection::TopToBottom)
        ->setMainAxisAlignment(MainAxisAlignment::End)
        ->setMainAxisScaling(AxisScaling::Fit)
        ->setGap(2.5f)
        ->ignoreInvisibleChildren(false)
        ->setPadding({0.f, 10.f, 0.f, 10.f})
    );

    m_mainLayer->addChild(m_scrollLayer);

    loadSettings();

    return true;
}

void SettingsPopup::loadSettings() {
    
    auto modjson = Mod::get()->getRuntimeInfo();
    auto& settings = modjson["settings"];

    for (auto& [key, value] : settings) {
        auto setting = Mod::get()->getSetting(key);
        if (!setting) continue;

        auto type = value["type"].asString().unwrapOrDefault();

        if (type.empty()) continue;

        auto node = tinker::settings::SettingNodeRegistry::get()->create(type, m_scrollLayer->getContentWidth(), setting);
        if (!node) continue;

        m_scrollLayer->addChild(node);
    }

    m_scrollLayer->getContentLayer()->updateLayout();
}