#include "ImprovedFontSelection.hpp"
#include <alphalaneous.alphas-ui-pack/include/API.hpp>

bool ImprovedFontSelection::onToggled(bool state) {
    return true;
}

bool ImprovedFontSelection::onSettingChanged(std::string_view key, const matjson::Value& value) {
    return true;
}

bool IFSSelectFontLayer::init(LevelEditorLayer* layer) {
    if (!SelectFontLayer::init(layer)) return false;
    
    m_mainLayer->setContentSize({400.f, 290.f});
    m_mainLayer->setPosition(getContentSize() / 2.f);
    m_mainLayer->ignoreAnchorPointForPosition(false);

    constexpr float centerOffset = 30.f;

    auto bg = m_mainLayer->getChildByType<CCScale9Sprite>(0);

    bg->setContentSize(m_mainLayer->getContentSize());
    bg->setPosition(m_mainLayer->getContentSize() / 2.f);
    bg->setID("background");

    auto leftBG = geode::NineSlice::create("GJ_square05.png");
    leftBG->setAnchorPoint({0.f, 0.f});
    leftBG->setContentSize({bg->getContentWidth() / 2.f + leftBG->getInsetRight() - centerOffset, bg->getContentHeight()});
    leftBG->getTopRight()->setVisible(false);
    leftBG->getBottomRight()->setVisible(false);
    leftBG->getRight()->setVisible(false);
    leftBG->setID("preview-background"_spr);

    bg->addChild(leftBG);

    getChildByType<CCLabelBMFont>(0)->setVisible(false);
    getChildByType<CCLabelBMFont>(1)->setVisible(false);

    m_buttonMenu->setContentSize(m_mainLayer->getContentSize());
    m_buttonMenu->setPosition(m_mainLayer->getContentSize() / 2.f);
    m_buttonMenu->ignoreAnchorPointForPosition(false);

    m_buttonMenu->getChildByType<CCMenuItemSpriteExtra>(0)->setPosition({m_buttonMenu->getContentWidth() / 2.f + m_mainLayer->getContentWidth() / 4.f - 5.f - centerOffset / 2.f, 25.f});
    m_buttonMenu->getChildByType<InfoAlertButton>(0)->setPosition({20.f, m_buttonMenu->getContentHeight() - 20.f});

    m_buttonMenu->getChildByType<CCMenuItemSpriteExtra>(2)->setVisible(false);
    m_buttonMenu->getChildByType<CCMenuItemSpriteExtra>(3)->setVisible(false);

    auto title = CCLabelBMFont::create("Select Font", "goldFont.fnt");
    title->setPosition({m_mainLayer->getContentWidth() / 2.f + m_mainLayer->getContentWidth() / 4.f - centerOffset / 2.f, m_mainLayer->getContentHeight() - 20.f});
    title->setScale(0.65f);
    title->setID("title"_spr);
    m_mainLayer->addChild(title);

    auto fields = m_fields.self();

    auto labelContainer = CCNode::create();
    labelContainer->setContentSize({m_mainLayer->getContentWidth() / 2.f - 30.f - centerOffset, m_mainLayer->getContentHeight() - 110.f});
    labelContainer->setAnchorPoint({0.f, 1.f});
    labelContainer->setPosition({15.f, m_mainLayer->getContentHeight() - 45.f});
    labelContainer->setLayout(SimpleRowLayout::create()
        ->setMainAxisScaling(AxisScaling::ScaleDown)
        ->setCrossAxisScaling(AxisScaling::ScaleDown)
        ->setMinRelativeScale(0.1f)
    );
    labelContainer->setID("label-container"_spr);

    fields->m_exampleLabel = CCLabelBMFont::create("The quick brown fox jumps over the lazy dog!", fontForID(m_font).c_str());
    fields->m_exampleLabel->setScale(0.6f);
    fields->m_exampleLabel->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
    fields->m_exampleLabel->setWidth(labelContainer->getContentWidth());
    fields->m_exampleLabel->setAnchorPoint({0.f, 1.f});
    fields->m_exampleLabel->setPosition({0.f, labelContainer->getContentHeight()});
    fields->m_exampleLabel->setID("preview-label"_spr);

    labelContainer->addChild(fields->m_exampleLabel);
    m_mainLayer->addChild(labelContainer);

    labelContainer->updateLayout();

    auto textInput = geode::TextInput::create(m_mainLayer->getContentWidth() / 2.f - 30.f - centerOffset, "Preview", "chatFont.fnt");
    textInput->setString(fields->m_exampleLabel->getString());
    textInput->setCallback([fields] (const std::string& str) {
        fields->m_exampleLabel->setString(str.c_str());
        fields->m_exampleLabel->getParent()->updateLayout();
    });

    textInput->setCommonFilter(CommonFilter::Any);
    textInput->setAnchorPoint({0.f, 0.f});
    textInput->setPosition({15.f, 15.f});
    textInput->setID("preview-input"_spr);

    m_mainLayer->addChild(textInput);

    auto scrollBG = CCLayerColor::create({0, 0, 0, 100});
    scrollBG->ignoreAnchorPointForPosition(false);
    scrollBG->setID("scroll-background"_spr);
    scrollBG->setAnchorPoint({0.f, 0.5f});
    scrollBG->setContentSize({m_mainLayer->getContentWidth() / 2.f - 3.f + centerOffset, m_mainLayer->getContentHeight() - 90.f});
    scrollBG->setPosition({m_mainLayer->getContentWidth() / 2.f - centerOffset, m_mainLayer->getContentHeight() / 2.f + 5.f});

    m_mainLayer->addChild(scrollBG);

    auto scrollLayer = alpha::ui::AdvancedScrollLayer::create(scrollBG->getContentSize());
    scrollLayer->setAnchorPoint({0.f, 0.f});
    scrollLayer->getContentLayer()->setLayout(ScrollLayer::createDefaultListLayout(0));
    scrollLayer->setID("font-scroll-layer"_spr);

    for (int i = 0; i < 60; i++) {
        auto font = FontContainer::create(i, scrollLayer->getContentWidth(), fontNameForID(i), fontForID(i), this);
        fields->m_fontContainers.push_back(font);

        scrollLayer->getContentLayer()->addChild(font);
    }

    scrollLayer->getContentLayer()->updateLayout();

    scrollBG->addChild(scrollLayer);

    auto scrollbar = alpha::ui::AdvancedScrollBar::create(scrollLayer, alpha::ui::ScrollOrientation::VERTICAL);
    auto style = alpha::ui::RoundedScrollStyle();
    style.m_track = [] {
        auto track = alpha::ui::RoundedScrollTrack::create();
        track->setClickColor({0, 0, 0, 0});
        track->setBackgroundColor({0, 0, 0, 0});
        return track;
    };

    scrollbar->setStyle(style);
    scrollbar->setPositionX(scrollbar->getPositionX() - 16.f);
    scrollbar->setID("font-scroll-bar"_spr);

    scrollBG->addChild(scrollbar);
    
    return true;
}

std::string IFSSelectFontLayer::fontForID(int id) {
    std::stringstream fntFile;

    if (id == 0) {
        fntFile << "bigFont.fnt";
    }
    else if (id < 10) {
        fntFile << "gjFont0";
        fntFile << id;
        fntFile << ".fnt";
    }
    else {
        fntFile << "gjFont";
        fntFile << id;
        fntFile << ".fnt";
    }

    return fntFile.str();
}

ZStringView IFSSelectFontLayer::fontNameForID(int id) {
    static constexpr std::array fontNames = {
        "Pusab", "TRS Million",
        "SF Distant Galaxy", "Relish Gargler", "Early GameBoy", "Good Dog",
        "Dancing Script", "Stencilia", "Xirod", "Minercraftory",
        "Super Mario 256", "Ketchum", "Gargle", "Amatic",
        "Cartwheel", "Mothproof Script", "Lemon Milk Bold", "Lemon Milk",
        "Minecraft", "Optimus Princeps", "Autolova", "Karate",
        "A Annyeong Haseyo", "Ausweis Hollow", "Gypsy Curse", "Magic School Two",
        "Old English Five", "Yeah Papa", "Ninja Naruto", "Metal Lord",
        "Drip Drop", "Electroharmonix", "Aladin", "Creepster",
        "Call Of Ops Duty", "BlocParty", "Astron Boy", "Osaka Sans Serif",
        "Some Time Later", "Fatboy Slim BLTC BRK", "Moria Citadel", "Rise of Kingdom",
        "Fantaisie Artistique", "Edge of the Galaxy", "Wash Your Hand", "Bitwise",
        "Foul Fiend", "Nandaka Western", "Evil Empire", "Comical Cartoon",
        "Carton Six", "Assassin Ninja", "Public Pixel", "New Walt Disney UI",
        "Random 5", "Crafting Lesson", "Game Of Squids", "Monster Game",
        "Lo-Sumires", "Gewtymol"
    };

    return fontNames[id];
}

void IFSSelectFontLayer::updateFont(int id) {
    auto fields = m_fields.self();
    fields->m_exampleLabel->setFntFile(fontForID(id).c_str());
    fields->m_exampleLabel->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
    fields->m_exampleLabel->getParent()->updateLayout();

    for (auto fontContainer : fields->m_fontContainers) {
        fontContainer->toggle(fontContainer->getFontID() == id);
    }
    m_font = id;

    GameManager::get()->m_levelEditorLayer->updateLevelFont(m_font);
    EditorUI::get()->resetSelectedObjectsColor();
}

FontContainer* FontContainer::create(int id, float width, ZStringView text, ZStringView fontFile, IFSSelectFontLayer* fontLayer) {
    auto ret = new FontContainer();
    if (ret->init(id, width, text, fontFile, fontLayer)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool FontContainer::init(int id, float width, ZStringView text, ZStringView fontFile, IFSSelectFontLayer* fontLayer) {
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
        m_fontLayer->updateFont(getFontID());
    });
    m_toggle->setID("use-toggle"_spr);

    if (fontLayer->m_font == id) {
        m_toggle->toggle(true);
    }

    m_toggle->setPosition({menu->getContentWidth() - m_toggle->getContentWidth() / 2.f - 12.f, menu->getContentHeight() / 2.f});
    m_toggle->m_notClickable = true;

    menu->addChild(m_toggle);

    addChild(menu);

    return true;
}

void FontContainer::setVisible(bool visible) {
    CCNode::setVisible(visible);

    if (visible && !m_label) {
        m_label = CCLabelBMFont::create(m_text.c_str(), m_fontFile.c_str());
        m_label->setAnchorPoint({0.f, 0.5f});
        m_label->setScale(0.6f);
        m_label->limitLabelWidth(getContentWidth() - 70.f, 0.6f, 0.1f);
        m_label->setPosition({5.f, getContentHeight() / 2.f});
        m_label->setID("font-name-label"_spr);
        addChild(m_label);

        auto idLabel = CCLabelBMFont::create(fmt::format("({})", m_id + 1).c_str(), "chatFont.fnt");
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