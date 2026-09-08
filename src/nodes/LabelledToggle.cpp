#include "nodes/LabelledToggle.hpp"

namespace tinker::ui {

LabelledToggle* LabelledToggle::create(ZStringView text, geode::Function<void(bool state)> callback) {
    auto ret = new LabelledToggle();
    if (ret->init(text, std::move(callback))) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool LabelledToggle::init(ZStringView text, geode::Function<void(bool state)> callback) {
    m_callback = std::move(callback);
    setAnchorPoint({0.5f, 0.5f});
    ignoreAnchorPointForPosition(false);
    setContentSize({125.f, 22.f});

    setLayout(SimpleRowLayout::create()
        ->setGap(8.f)
        ->setMainAxisScaling(AxisScaling::None)
        ->setMainAxisAlignment(MainAxisAlignment::Start)
    );

    m_toggler = CCMenuItemExt::createTogglerWithStandardSprites(0.7f, [this] (CCMenuItemToggler* toggle) {
        m_callback(!toggle->isToggled());
    });
    m_toggler->setID("toggler"_spr);

    auto menu = CCMenu::create();
    menu->setAnchorPoint({0.5f, 0.5f});
    menu->ignoreAnchorPointForPosition(false);
    menu->setContentSize(m_toggler->getContentSize());
    menu->setID("toggler-menu"_spr);

    m_toggler->setPosition(menu->getContentSize() / 2.f);

    menu->addChild(m_toggler);

    addChild(menu);

    auto label = geode::Label::create(text, "bigFont.fnt");
    label->setScale(0.35f);
    label->setMaxWidth(250.f);
    label->setID("toggler-label"_spr);

    addChild(label);

    updateLayout();

    return true;
}

CCMenuItemToggler* LabelledToggle::getToggler() {
    return m_toggler;
}

bool LabelledToggle::isToggled() {
    return m_toggler->isToggled();
}

void LabelledToggle::toggle(bool on) {
    m_toggler->toggle(on);
}

}