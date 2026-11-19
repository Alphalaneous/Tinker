#include "modules/ImprovedFontSelection.hpp"
#include "Geode/ui/Label.hpp"
#include "Geode/ui/TextInput.hpp"
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <alphalaneous.alphas-ui-pack/API.hpp>
#include <string>

using namespace geode::prelude;

bool ImprovedFontSelection::onToggled(bool state) {
    return true;
}

bool ImprovedFontSelection::onSettingChanged(std::string_view key, const matjson::Value& value) {
    return true;
}

bool IFSSelectFontLayer2::init(LevelEditorLayer* layer) {
    if (!SelectFontLayer2::init(layer)) return false;
    
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

    auto sideShadow = CCSprite::createWithSpriteFrameName("d_gradient_c_01_001.png");
    sideShadow->setOpacity(30);
    sideShadow->setZOrder(10);
    sideShadow->setRotation(-90.f);
    sideShadow->setColor({0, 0, 0});
    sideShadow->setAnchorPoint({0.5f, 0.f});
    sideShadow->setScaleY(0.075f);
    sideShadow->setScaleX((leftBG->getContentHeight() - 6.f) / sideShadow->getContentWidth());
    sideShadow->setPosition({leftBG->getContentWidth() - leftBG->getInsetRight(), leftBG->getContentHeight() / 2.f});
    sideShadow->setID("right-preview-shadow"_spr);

    leftBG->addChild(sideShadow);

    auto list = m_mainLayer->getChildByType<GJCommentListLayer>();
    if (list) {
        list->setVisible(false);
        // robs scroll bs messes with it even if not visible
        list->setPosition({FLT_MAX, FLT_MAX});
    }

    auto oldTitle = m_mainLayer->getChildByType<CCLabelBMFont>();
    if (oldTitle) {
        oldTitle->setVisible(false);
    }

    auto oldCloseBtn = m_buttonMenu->getChildByType<CCMenuItemSpriteExtra>();
    if (oldCloseBtn) {
        oldCloseBtn->setVisible(false);
    }

    m_buttonMenu->setContentSize(m_mainLayer->getContentSize());
    m_buttonMenu->setPosition(m_mainLayer->getContentSize() / 2.f);
    m_buttonMenu->ignoreAnchorPointForPosition(false);

    auto okBtnSpr = ButtonSprite::create("OK", 40, 0, 0.8f, true, "goldFont.fnt", "GJ_button_01.png", 30.f);
    auto okBtn = geode::Button::createWithNode(okBtnSpr, [this] (auto sender) {
        keyBackClicked();
    });

    okBtn->setPosition({m_mainLayer->getContentWidth() / 2.f + m_mainLayer->getContentWidth() / 4.f - 5.f - centerOffset / 2.f, 27.f});
    okBtn->setID("ok-button"_spr);

    m_mainLayer->addChild(okBtn);

    auto title = geode::Label::create("Select Font", "goldFont.fnt");
    title->setPosition({m_mainLayer->getContentWidth() / 2.f + m_mainLayer->getContentWidth() / 4.f - centerOffset / 2.f, m_mainLayer->getContentHeight() - 20.f});
    title->setScale(0.65f);
    title->setID("title"_spr);
    m_mainLayer->addChild(title);

    auto fields = m_fields.self();

    fields->m_font = layer->m_levelSettings->m_fontIndex;

    auto previewStr = ImprovedFontSelection::getSaved<Save::Global, std::string, "preview-text">("The quick brown fox jumps over the lazy dog!");

    fields->m_exampleLabel = geode::TextInput::createMultiline({m_mainLayer->getContentWidth() / 2.f - 55.f, m_mainLayer->getContentHeight() - 30.f}, "Enter Text...", fontForID(fields->m_font).c_str());
    fields->m_exampleLabel->setFontScale(0.5f);
    fields->m_exampleLabel->setPlaceholderScale(0.5f);
    fields->m_exampleLabel->setCallback([this] (const std::string& str) {
        ImprovedFontSelection::setSaved<Save::Global, "preview-text">(str);
    });
    fields->m_exampleLabel->setString(previewStr);
    fields->m_exampleLabel->setAnchorPoint({0.f, 1.f});
    fields->m_exampleLabel->setPosition({14.f, m_mainLayer->getContentHeight() - 15.f});
    fields->m_exampleLabel->setID("preview-label"_spr);

    m_mainLayer->addChild(fields->m_exampleLabel);

    auto scrollBG = CCLayerColor::create({0, 0, 0, 100});
    scrollBG->ignoreAnchorPointForPosition(false);
    scrollBG->setID("scroll-background"_spr);
    scrollBG->setAnchorPoint({0.f, 0.5f});
    scrollBG->setContentSize({m_mainLayer->getContentWidth() / 2.f - 3.f + centerOffset, m_mainLayer->getContentHeight() - 90.f});
    scrollBG->setPosition({m_mainLayer->getContentWidth() / 2.f - centerOffset, m_mainLayer->getContentHeight() / 2.f + 5.f});

    m_mainLayer->addChild(scrollBG);

    auto bottomShadow = CCSprite::createWithSpriteFrameName("d_gradient_c_01_001.png");
    bottomShadow->setOpacity(30);
    bottomShadow->setZOrder(10);
    bottomShadow->setColor({0, 0, 0});
    bottomShadow->setAnchorPoint({0.f, 0.f});
    bottomShadow->setScaleY(0.075f);
    bottomShadow->setScaleX(scrollBG->getContentWidth() / bottomShadow->getContentWidth());
    bottomShadow->setPositionY(0.f);
    bottomShadow->setID("bottom-scroll-shadow"_spr);

    scrollBG->addChild(bottomShadow);

    auto topShadow = CCSprite::createWithSpriteFrameName("d_gradient_c_01_001.png");
    topShadow->setOpacity(30);
    topShadow->setZOrder(10);
    topShadow->setFlipY(true);
    topShadow->setColor({0, 0, 0});
    topShadow->setAnchorPoint({0.f, 1.f});
    topShadow->setScaleY(0.075f);
    topShadow->setScaleX(scrollBG->getContentWidth() / topShadow->getContentWidth());
    topShadow->setPositionY(scrollBG->getScaledContentHeight());
    topShadow->setID("top-scroll-shadow"_spr);

    scrollBG->addChild(topShadow);

    auto scrollLayer = alpha::ui::AdvancedScrollLayer::create(scrollBG->getContentSize());
    scrollLayer->setAnchorPoint({0.f, 0.f});
    scrollLayer->getContentLayer()->setLayout(ScrollLayer::createDefaultListLayout(0));
    scrollLayer->setID("font-scroll-layer"_spr);

    for (int i = 0; i < 60; i++) {
        auto font = tinker::ui::FontContainer::create(i, scrollLayer->getContentWidth(), fontNameForID(i), fontForID(i), this);
        fields->m_fontContainers.push_back(font);

        scrollLayer->getContentLayer()->addChild(font);
    }

    scrollLayer->getContentLayer()->updateLayout();

    scrollBG->addChild(scrollLayer);

    auto scrollbar = alpha::ui::AdvancedScrollBar::create(scrollLayer, alpha::ui::ScrollOrientation::Vertical);
    auto style = alpha::ui::RoundedScrollStyle();
    style.m_track = [] {
        auto track = alpha::ui::RoundedScrollTrack::create();
        track->setClickColor({0, 0, 0, 0});
        track->setBackgroundColor({0, 0, 0, 0});
        return track;
    };

    scrollbar->setStyle(style);
    scrollbar->setPositionX(scrollbar->getPositionX() - 14.f);
    scrollbar->setID("font-scroll-bar"_spr);
    scrollbar->setContentWidth(10.f);

    scrollBG->addChild(scrollbar);

    for (auto fontContainer : fields->m_fontContainers) {
        if (fontContainer->getFontID() == fields->m_font) {
            scrollLayer->setScrollY(scrollLayer->getContentLayer()->getContentHeight() - fontContainer->getPositionY() - scrollLayer->getContentHeight() / 2.f);
            break;
        }
    }
    
    return true;
}

std::string IFSSelectFontLayer2::fontForID(int id) {
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

ZStringView IFSSelectFontLayer2::fontNameForID(int id) {
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

void IFSSelectFontLayer2::updateFont(int id) {
    auto fields = m_fields.self();
    auto font = fontForID(id);

    fields->m_exampleLabel->getPlaceholderLabel()->setFont(font);
    fields->m_exampleLabel->getContentLabel()->setFont(font);
    fields->m_exampleLabel->setFontScale(0.5f);
    fields->m_exampleLabel->setPlaceholderScale(0.5f);

    fields->m_exampleLabel->getParent()->updateLayout();

    for (auto fontContainer : fields->m_fontContainers) {
        fontContainer->toggle(fontContainer->getFontID() == id);
    }
    fields->m_font = id;

    GameManager::get()->m_levelEditorLayer->updateLevelFont(fields->m_font);
    EditorUI::get()->resetSelectedObjectsColor();
}