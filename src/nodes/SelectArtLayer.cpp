#include "nodes/SelectArtLayer.hpp"
#include "nodes/ColorChannelSprite.hpp"
#include "utils/Constants.hpp"
#include "utils/Utils.hpp"

namespace tinker::ui {

SelectArtLayer* SelectArtLayer::create(LevelEditorLayer* editorLayer, LevelSettingsLayer* levelSettingsLayer, SelectArtType type) {
    auto ret = new SelectArtLayer();
    if (ret->init(editorLayer, levelSettingsLayer, type)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool SelectArtLayer::init(LevelEditorLayer* editorLayer, LevelSettingsLayer* levelSettingsLayer, SelectArtType type) {
    if (!geode::Popup::init({450, 290})) return false;
    m_levelSettingsLayer = levelSettingsLayer;
    m_editorLayer = editorLayer;
    m_type = type;

    m_noElasticity = true;
    m_closeBtn->removeFromParent();

    getChildrenExt();

    constexpr float padding = 3.f;

    auto levelSettings = editorLayer->m_levelSettings;

    m_backgroundID = std::max(1, levelSettings->m_backgroundIndex);
    m_groundID = std::max(1, levelSettings->m_groundIndex);
    m_groundLineID = std::max(1, levelSettings->m_groundLineIndex);
    m_middlegroundID = levelSettings->m_middleGroundIndex;

    m_stylePreview = ArtPreview::create({m_size.width / 2.f - padding, m_size.height - padding * 2.f}, m_backgroundID, m_groundID, m_groundLineID, m_middlegroundID);
    m_stylePreview->setAnchorPoint({0.f, 0.5f});
    m_stylePreview->setPosition({padding, m_size.height / 2.f});
    m_stylePreview->setID("style-preview"_spr);

    updateColors();

    m_mainLayer->addChild(m_stylePreview);

    auto foregroundBorder = geode::NineSlice::create("GJ_square07.png");
    foregroundBorder->setContentSize(m_size);
    foregroundBorder->setZOrder(10000);
    foregroundBorder->setPosition(m_size / 2.f);
    foregroundBorder->setID("border"_spr);

    m_mainLayer->addChild(foregroundBorder);

    constexpr float centerOffset = 0.f;

    auto title = CCLabelBMFont::create("Select Style", "goldFont.fnt");
    title->setPosition({m_size.width / 2.f + m_size.width / 4.f - centerOffset / 2.f, m_size.height - 20.f});
    title->setScale(0.65f);
    title->setID("title"_spr);

    m_mainLayer->addChild(title);

    auto btnContainer = CCNode::create();
    btnContainer->setID("button-container"_spr);
    btnContainer->setAnchorPoint({0.5f, 0.f});
    btnContainer->setLayout(SimpleRowLayout::create()
        ->setGap(5.f)
        ->setMainAxisScaling(AxisScaling::Grow)
        ->setCrossAxisScaling(AxisScaling::Grow)
    );
    btnContainer->setPosition({m_size.width / 2.f + m_size.width / 4.f - centerOffset / 2.f, 10.f});
    btnContainer->setZOrder(10);

    auto okBtnSpr = ButtonSprite::create("OK", 40, 0, 0.8f, true, "goldFont.fnt", "GJ_button_01.png", 30.f);
    auto okBtn = geode::Button::createWithNode(okBtnSpr, [this] (auto sender) {
        keyBackClicked();
    });

    okBtn->setID("ok-button"_spr);

    btnContainer->addChild(okBtn);

    auto savePresetSpr = ButtonSprite::create("Save Preset", 60, 0, 0.8f, true, "goldFont.fnt", "GJ_button_02.png", 30.f);
    auto savePresetBtn = geode::Button::createWithNode(savePresetSpr, [this] (auto sender) {

    });

    savePresetBtn->setID("save-preset-button"_spr);

    //btnContainer->addChild(savePresetBtn);
    btnContainer->updateLayout();

    m_mainLayer->addChild(btnContainer);

    m_scrollBG = CCLayerColor::create({0, 0, 0, 100});
    m_scrollBG->ignoreAnchorPointForPosition(false);
    m_scrollBG->setID("scroll-background"_spr);
    m_scrollBG->setAnchorPoint({0.f, 1.f});
    m_scrollBG->setContentSize({m_size.width / 2.f - 3.f + centerOffset, m_size.height - 120.f});
    m_scrollBG->setPosition({m_size.width / 2.f - centerOffset, m_size.height - 70.f});

    m_mainLayer->addChild(m_scrollBG);

    m_scrollLayer = alpha::ui::AdvancedScrollLayer::create(m_scrollBG->getContentSize());
    m_scrollLayer->setAnchorPoint({0.f, 0.f});
    m_scrollLayer->getContentLayer()->setLayout(RowLayout::create()
        ->setGrowCrossAxis(true)
        ->setPadding({5.f, 5.f, 5.f, 5.f})
        ->setAutoScale(false)
        ->ignoreInvisibleChildren(false)
    );
    m_scrollLayer->setID("scroll-layer"_spr);
    #ifdef GEODE_IS_MOBILE
    m_scrollLayer->setScrollDelta(1.5f);
    #else
    m_scrollLayer->setScrollDelta(1.f);
    #endif

    m_scrollBG->addChild(m_scrollLayer);

    auto scrollbar = alpha::ui::AdvancedScrollBar::create(m_scrollLayer, alpha::ui::ScrollOrientation::VERTICAL);
    auto style = alpha::ui::RoundedScrollStyle();
    style.m_track = [] {
        auto track = alpha::ui::RoundedScrollTrack::create();
        track->setClickColor({0, 0, 0, 0});
        track->setBackgroundColor({0, 0, 0, 0});
        return track;
    };

    scrollbar->setStyle(style);
    scrollbar->setPositionX(scrollbar->getPositionX() - 14.f);
    scrollbar->setContentWidth(10.f);
    scrollbar->setID("scroll-bar"_spr);

    m_scrollBG->addChild(scrollbar);

    m_lineContainer = CCNode::create();
    m_lineContainer->setID("line-container"_spr);
    m_lineContainer->setVisible(false);
    m_lineContainer->setAnchorPoint({0.5f, 0.5f});
    m_lineContainer->setPosition({m_size.width / 2.f + m_size.width / 4.f - centerOffset, m_scrollBG->boundingBox().getMinY() + 10.f});
    m_lineContainer->setLayout(SimpleRowLayout::create()
        ->setGap(5.f)
        ->setMainAxisScaling(AxisScaling::Fit)
        ->setCrossAxisScaling(AxisScaling::Fit)
    );
    
    m_mainLayer->addChild(m_lineContainer);

    auto lineLabel = CCLabelBMFont::create("Line:", "goldFont.fnt");
    lineLabel->setID("line-label"_spr);
    lineLabel->setScale(0.5f);

    m_lineContainer->addChild(lineLabel);

    auto lineMenu = CCMenu::create();
    lineMenu->setID("line-menu"_spr);
    lineMenu->setAnchorPoint({0.5f, 0.5f});
    lineMenu->setScale(0.8f);
    lineMenu->setLayout(SimpleRowLayout::create()
        ->setGap(5.f)
        ->setMainAxisScaling(AxisScaling::Fit)
        ->setCrossAxisScaling(AxisScaling::Fit)
    );

    lineMenu->addChild(createLineToggler(1));
    lineMenu->addChild(createLineToggler(2));
    lineMenu->addChild(createLineToggler(3));

    lineMenu->updateLayout();
    m_lineContainer->addChild(lineMenu);

    m_lineContainer->updateLayout();

    auto togglerMenu = CCMenu::create();
    togglerMenu->setID("categories-container"_spr);
    togglerMenu->setZOrder(10);
    togglerMenu->setAnchorPoint({0.5f, 1.f});
    togglerMenu->ignoreAnchorPointForPosition(false);
    togglerMenu->setPosition({m_size.width / 2.f + m_size.width / 4.f -  - centerOffset, m_size.height - 40.f});

    togglerMenu->setLayout(SimpleRowLayout::create()
        ->setGap(5.f)
        ->setMainAxisScaling(AxisScaling::Fit)
        ->setCrossAxisScaling(AxisScaling::Fit)
    );

    togglerMenu->addChild(createTabToggler(SelectArtType::Background, "Background", "background-toggler"_spr, {0.6f, -1.f}));
    togglerMenu->addChild(createTabToggler(SelectArtType::Ground, "Ground", "ground-toggler"_spr, {0.6f, -0.7f}));
    togglerMenu->addChild(createTabToggler(SelectArtType::Middleground, "MIddleground", "middleground-toggler"_spr, {0.6f, -1.f}));

    togglerMenu->updateLayout();

    m_mainLayer->addChild(togglerMenu);

    for (int i = 1; i <= 59; i++) {
        m_artNodes[SelectArtType::Background].push_back(createArtButton(SelectArtType::Background, i));
    }

    for (int i = 1; i <= 22; i++) {
        m_artNodes[SelectArtType::Ground].push_back(createArtButton(SelectArtType::Ground, i));
    }

    for (int i = 0; i <= 3; i++) {
        m_artNodes[SelectArtType::Middleground].push_back(createArtButton(SelectArtType::Middleground, i));
    }

    loadType(type);

    auto colorContainer = CCNode::create();
    colorContainer->setID("color-container"_spr);
    colorContainer->setAnchorPoint({0.f, 0.f});
    colorContainer->ignoreAnchorPointForPosition(false);
    colorContainer->setPosition({3.f, 3.f});
    colorContainer->setContentSize({40.f, m_stylePreview->getContentHeight()});
    colorContainer->setZOrder(1);
    colorContainer->setLayout(SimpleColumnLayout::create()
        ->setGap(5.f)
        ->setMainAxisDirection(AxisDirection::TopToBottom)
    );

    colorContainer->addChild(createColorButton(constants::color_channels::Background));
    colorContainer->addChild(createColorButton(constants::color_channels::Ground1));
    colorContainer->addChild(createColorButton(constants::color_channels::Ground2));
    colorContainer->addChild(createColorButton(constants::color_channels::Line));
    colorContainer->addChild(createColorButton(constants::color_channels::Middleground1));
    colorContainer->addChild(createColorButton(constants::color_channels::Middleground2));
    colorContainer->addChild(createColorButton(constants::color_channels::Obj));
    colorContainer->addChild(createColorButton(constants::color_channels::Line3D));

    colorContainer->updateLayout();

    m_mainLayer->addChild(colorContainer);

    return true;
}

void SelectArtLayer::updateColors() {
    auto lineAction = m_editorLayer->m_levelSettings->m_effectManager->getColorAction(constants::color_channels::Line);

    m_stylePreview->updateBGColor(tinker::utils::getRealizedColor(constants::color_channels::Background));
    m_stylePreview->updateGroundColor(
        tinker::utils::getRealizedColor(constants::color_channels::Ground1),
        tinker::utils::getRealizedColor(constants::color_channels::Ground2),
        tinker::utils::getRealizedColor(constants::color_channels::Line),
        lineAction->m_fromOpacity * 255,
        lineAction->m_blending
    );

    auto mg1Action = m_editorLayer->m_levelSettings->m_effectManager->getColorAction(constants::color_channels::Middleground1);
    auto mg2Action = m_editorLayer->m_levelSettings->m_effectManager->getColorAction(constants::color_channels::Middleground2);

    m_stylePreview->updateMiddlegroundColor(
        tinker::utils::getRealizedColor(constants::color_channels::Middleground1),
        mg1Action->m_fromOpacity * 255,
        mg1Action->m_blending,
        tinker::utils::getRealizedColor(constants::color_channels::Middleground2),
        mg2Action->m_fromOpacity * 255,
        mg2Action->m_blending
    );

    for (auto spr : m_colorSprites) {
        spr->updateSprite();
    }
}

geode::Button* SelectArtLayer::createColorButton(int colorID) {
    auto colorSpr = tinker::ui::ColorChannelSprite::create(colorID);
    colorSpr->setID("color-sprite"_spr);

    m_colorSprites.push_back(colorSpr);
    auto btn = geode::Button::createWithNode(colorSpr, [this, colorID] (auto sender) {
        auto action = m_editorLayer->m_levelSettings->m_effectManager->getColorAction(colorID);
        auto popup = ColorSelectPopup::create(action);
        popup->m_delegate = this;
        popup->show();
        popup->setZOrder(CCScene::get()->getHighestChildZ() + 1);
    });
    btn->setID(fmt::format("color-{}-btn"_spr, geode::utils::string::toLower(colorSpr->textForID(colorID, false))));

    return btn;
}

void SelectArtLayer::colorSelectClosed(cocos2d::CCNode* popup) {
    updateColors();
    m_levelSettingsLayer->updateColorSprites();
}

CCMenuItemToggler* SelectArtLayer::createLineToggler(int type) {
    auto onSpr = ButtonSprite::create(numToString(type).c_str(), 20, 0, 0.5f, true, "bigFont.fnt", "GJ_button_02.png", 20.f);
    auto offSpr = ButtonSprite::create(numToString(type).c_str(), 20, 0, 0.5f, true, "bigFont.fnt", "GJ_button_04.png", 20.f);

    onSpr->m_label->setPosition(onSpr->m_label->getPosition() + CCPoint{0.6f, -0.5f});
    offSpr->m_label->setPosition(offSpr->m_label->getPosition() + CCPoint{0.6f, -0.5f});

    auto toggler = CCMenuItemExt::createToggler(onSpr, offSpr, [this, type] (auto toggler) {
        m_groundLineID = type;
        m_groundChanged = true;
        m_stylePreview->updateGround(m_groundID, type);

        for (auto tabToggler : m_lineTogglers) {
            if (tabToggler == toggler) continue;
            tabToggler->toggle(false);
        }
        toggler->toggle(true);
    });
    toggler->setTag(static_cast<int>(type));
    toggler->setID(fmt::format("line-{}"_spr, type));
    toggler->m_notClickable = true;

    if (type == m_groundLineID) {
        toggler->toggle(true);
    }

    m_lineTogglers.push_back(toggler);

    return toggler;
}

CCMenuItemToggler* SelectArtLayer::createTabToggler(SelectArtType type, ZStringView text, ZStringView id, const CCPoint& offset) {
    auto onSpr = ButtonSprite::create(text.c_str(), 48, 0, 0.7f, true, "bigFont.fnt", "GJ_button_01.png", 20.f);
    auto offSpr = ButtonSprite::create(text.c_str(), 48, 0, 0.7f, true, "bigFont.fnt", "GJ_button_04.png", 20.f);

    onSpr->m_label->setPosition(onSpr->m_label->getPosition() + offset);
    offSpr->m_label->setPosition(offSpr->m_label->getPosition() + offset);

    auto toggler = CCMenuItemExt::createToggler(onSpr, offSpr, [this, type] (auto toggler) {
        loadType(type);
        m_type = type;

        for (auto tabToggler : m_tabTogglers) {
            if (tabToggler == toggler) continue;
            tabToggler->toggle(false);
        }
        toggler->toggle(true);
    });
    toggler->setTag(static_cast<int>(type));
    toggler->setID(id);
    toggler->m_notClickable = true;

    if (type == m_type) {
        toggler->toggle(true);
    }

    m_tabTogglers.push_back(toggler);

    return toggler;
}

geode::Button* SelectArtLayer::createArtButton(SelectArtType type, int id) {
    std::string prefix = "bg";
    switch (type) {
        case SelectArtType::Background: {
            prefix = "bg";
            break;
        }
        case SelectArtType::Ground: {
            prefix = "g";
            break;
        }
        case SelectArtType::Middleground: {
            prefix = "mg";
            break;
        }
        default: {
            break;
        }
    }

    auto btn = geode::Button::createWithSpriteFrameName(fmt::format("{}Icon_{:02d}_001.png", prefix, id), [this, type, id] (auto sender) {
        switch (type) {
            case SelectArtType::Background: {
                m_stylePreview->updateBG(id);
                m_backgroundID = id;
                m_backgroundChanged = true;
                break;
            }
            case SelectArtType::Ground: {
                m_stylePreview->updateGround(id, m_groundLineID);
                m_groundID = id;
                m_groundChanged = true;
                break;
            }
            case SelectArtType::Middleground: {
                m_stylePreview->updateMiddleground(id);
                m_middlegroundID = id;
                m_middlegroundChanged = true;
                break;
            }
            default: {
                break;
            }
        }

        updateSprites(type);
    });

    btn->setCascadeColorEnabled(true);
    btn->setColor({125, 125, 125});
    btn->setTag(id);
    btn->setZOrder(id);
    btn->setScale(0.75f);

    return btn;
}

void SelectArtLayer::keyBackClicked() {
    if (m_backgroundChanged) {
        if (m_editorLayer->m_background) {
            m_editorLayer->m_background->removeFromParent();
            m_editorLayer->m_background = nullptr;
        }
        m_editorLayer->m_levelSettings->m_backgroundIndex = m_backgroundID;
        GameManager::get()->loadBackground(m_editorLayer->m_levelSettings->m_backgroundIndex);
        
        auto newSpr = CCSpriteFrameCache::get()->spriteFrameByName(fmt::format("bgIcon_{:02d}_001.png", m_backgroundID).c_str());
        m_levelSettingsLayer->m_backgroundSprite->setDisplayFrame(newSpr);
        m_editorLayer->createBackground(m_editorLayer->m_levelSettings->m_backgroundIndex);
    }
    if (m_groundChanged) {
        if (m_editorLayer->m_groundLayer) {
            m_editorLayer->m_groundLayer->removeFromParent();
            m_editorLayer->m_groundLayer = nullptr;
        }
        if (m_editorLayer->m_groundLayer2) {
            m_editorLayer->m_groundLayer2->removeFromParent();
            m_editorLayer->m_groundLayer2 = nullptr;
        }

        m_editorLayer->m_levelSettings->m_groundIndex = m_groundID;
        m_editorLayer->m_levelSettings->m_groundLineIndex = m_groundLineID;

        GameManager::get()->loadGround(m_editorLayer->m_levelSettings->m_groundIndex);

        auto newSpr = CCSpriteFrameCache::get()->spriteFrameByName(fmt::format("gIcon_{:02d}_001.png", m_groundID).c_str());
        m_levelSettingsLayer->m_groundSprite->setDisplayFrame(newSpr);
        m_editorLayer->createGroundLayer(m_editorLayer->m_levelSettings->m_groundIndex, m_editorLayer->m_levelSettings->m_groundLineIndex);
    }
    if (m_middlegroundChanged) {
        if (m_editorLayer->m_middleground) {
            m_editorLayer->m_middleground->removeFromParent();
            m_editorLayer->m_middleground = nullptr;
        }

        m_editorLayer->m_levelSettings->m_middleGroundIndex = m_middlegroundID;

        GameManager::get()->loadMiddleground(m_editorLayer->m_levelSettings->m_middleGroundIndex);

        auto newSpr = CCSpriteFrameCache::get()->spriteFrameByName(fmt::format("mgIcon_{:02d}_001.png", m_middlegroundID).c_str());
        m_levelSettingsLayer->m_middlegroundSprite->setDisplayFrame(newSpr);
        m_editorLayer->createMiddleground(m_editorLayer->m_levelSettings->m_middleGroundIndex);
    }
    m_scrollLayer->getContentLayer()->removeAllChildren();
    m_scrollLayer->removeFromParent();
    m_scrollLayer = nullptr;
    Popup::keyBackClicked();
}

void SelectArtLayer::updateSprites(SelectArtType type) {
    auto& artVec = m_artNodes[type];

    int id = 0;
    switch (type) {
        case SelectArtType::Background: {
            id = m_backgroundID;
            break;
        }
        case SelectArtType::Ground: {
            id = m_groundID;
            break;
        }
        case SelectArtType::Middleground: {
            id = m_middlegroundID;
            break;
        }
        default: {
            break;
        }
    }

    for (const auto& btn : artVec) {
        btn->setColor(btn->getTag() == id ? ccColor3B{255, 255, 255} : ccColor3B{125, 125, 125});
    }
}

void SelectArtLayer::loadType(SelectArtType type) {
    m_scrollLayer->getContentLayer()->removeAllChildren();

    constexpr float centerOffset = 0.f;
    auto contentSize = CCSize{m_size.width / 2.f - 3.f + centerOffset, m_size.height - 120.f};

    m_lineContainer->setVisible(type == SelectArtType::Ground);

    auto& artVec = m_artNodes[type];

    int id = 0;
    switch (type) {
        case SelectArtType::Background: {
            id = m_backgroundID;
            break;
        }
        case SelectArtType::Ground: {
            id = m_groundID;
            contentSize = CCSize{m_size.width / 2.f - 3.f + centerOffset, m_size.height - 140.f};
            break;
        }
        case SelectArtType::Middleground: {
            id = m_middlegroundID;
            break;
        }
        default: {
            break;
        }
    }

    m_scrollBG->setContentSize(contentSize);
    m_scrollLayer->setContentSize(m_scrollBG->getContentSize());

    for (const auto& btn : artVec) {
        m_scrollLayer->getContentLayer()->addChild(btn);
        btn->setColor(btn->getTag() == id ? ccColor3B{255, 255, 255} : ccColor3B{125, 125, 125});
    }

    m_scrollLayer->getContentLayer()->updateLayout();

    for (const auto& btn : artVec) {
        if (btn->getTag() == id) {
            float scrollY = m_scrollLayer->getContentLayer()->getContentHeight() - btn->getPositionY() - m_scrollLayer->getContentHeight() / 2.f;
            m_scrollLayer->setScrollY(scrollY);
            m_scrollLayer->forceCull();
            break;
        }
    }
}

}