#include "nodes/FontContainer.hpp"
#include "modules/ImprovedFontSelection.hpp"

namespace tinker::ui {

FontContainer* FontContainer::create(int id, float width, ZStringView text, ZStringView fontFile, SelectFontLayer* fontLayer) {
    auto ret = new FontContainer();
    if (ret->init(id, width, text, fontFile, fontLayer)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool FontContainer::init(int id, float width, ZStringView text, ZStringView fontFile, SelectFontLayer* fontLayer) {
    GLubyte opacity = (id % 2 == 0) ? 50 : 0;
    if (!CCLayerColor::initWithColor({0, 0, 0, opacity})) return false;

    setContentSize({width, 30.f});

    m_id = id;
    m_text = text;
    m_fontFile = fontFile;
    m_fontLayer = fontLayer;

    setID(fmt::format("font-{}"_spr, id));

    auto menu = CCMenu::create();
    menu->setContentSize(getContentSize());
    menu->ignoreAnchorPointForPosition(false);
    menu->setAnchorPoint({0.f, 0.f});
    menu->setPosition({0.f, 0.f});
    menu->setID("main-menu"_spr);

    m_toggle = CCMenuItemExt::createTogglerWithFrameName("GJ_selectSongOnBtn_001.png", "GJ_selectSongBtn_001.png", 0.5f, [this] (auto sender) {
        static_cast<IFSSelectFontLayer*>(m_fontLayer)->updateFont(getFontID());
    });
    m_toggle->setID("use-toggle"_spr);

    if (fontLayer->m_font == id) {
        m_toggle->toggle(true);
    }

    m_toggle->setPosition({menu->getContentWidth() - m_toggle->getContentWidth() / 2.f - 10.f, menu->getContentHeight() / 2.f});
    m_toggle->m_notClickable = true;

    menu->addChild(m_toggle);

    addChild(menu);

    return true;
}

void FontContainer::setVisible(bool visible) {
    CCNode::setVisible(visible);

    if (visible && !m_label) {
        m_label = geode::Label::create(m_text.c_str(), m_fontFile.c_str());
        m_label->setAnchorPoint({0.f, 0.5f});
        m_label->setScale(0.6f);
        m_label->setLimitLabelWidth(getContentWidth() - 70.f, 0.6f, 0.1f);
        m_label->setPosition({8.f, getContentHeight() / 2.f});
        m_label->setID("font-name-label"_spr);
        m_label->validate();
        addChild(m_label);

        auto idLabel = geode::Label::create(fmt::format("({})", m_id + 1).c_str(), "chatFont.fnt");
        idLabel->setAnchorPoint({0.f, 0.5f});
        idLabel->setOpacity(127);
        idLabel->setScale(0.5f);
        idLabel->setPosition({m_label->boundingBox().getMaxX() + 5.f, getContentHeight() / 2.f});
        idLabel->setID("font-id-label"_spr);
        addChild(idLabel);
    }
}

void FontContainer::toggle(bool toggle) {
    m_toggle->toggle(toggle);
}

int FontContainer::getFontID() {
    return m_id;
}

}