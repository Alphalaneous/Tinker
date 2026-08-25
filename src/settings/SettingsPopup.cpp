#include "settings/SettingsPopup.hpp"
#include "settings/SettingNodeRegistry.hpp"
#include "settings/SettingNode.hpp"
#include "settings/SettingsCache.hpp"
#include "settings/SupportButton.hpp"
#include "nodes/PopupBorder.hpp"
#include "utils/Utils.hpp"
#include <Geode/modify/SliderTouchLogic.hpp>
#include <alphalaneous.alphas_geode_utils/include/ObjectModify.hpp>

class TitleSettingNodeV3 : public CCNode {};

struct SliderStateChanged final : Event<SliderStateChanged, bool(bool started)> {
    using Event::Event;
};

namespace tinker::ui {

SettingsPopup* SettingsPopup::create(bool useGeodeTheme) {
    auto ret = new SettingsPopup();
    if (ret->init(useGeodeTheme)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool SettingsPopup::init(bool useGeodeTheme) {
    if (!Popup::init({450, 280}, "GJ_square01.png")) return false;
    m_bgSprite->removeFromParent();
    m_noElasticity = true;

    if (useGeodeTheme) {
        auto circleButtonSprite = CircleButtonSprite::createWithSpriteFrameName("geode.loader/close.png", 0.8f, geode::CircleBaseColor::DarkPurple);
        circleButtonSprite->setScale(0.85f);
        m_closeBtn->setSprite(circleButtonSprite);
    }

    constexpr float topOffset = 3.f;
    constexpr float bottomOffset = 3.f;
    constexpr float leftOffset = 3.f;
    constexpr float rightOffset = 3.f;
    constexpr float settingsWidth = 320.f;

    auto mainColor = useGeodeTheme ? ccColor4B{39, 39, 54, 255} : ccColor4B{84, 84, 84, 255};
    auto secondaryColor = useGeodeTheme ? ccColor4B{26, 25, 34, 255} : ccColor4B{153, 85, 51, 255};
    auto bottomColor = useGeodeTheme ? ccColor3B{21, 19, 23} : ccColor3B{17, 17, 17};

    auto topLeftColor = useGeodeTheme ? ccColor3B{66, 61, 78} : ccColor3B{140, 140, 140};
    auto topRightColor = useGeodeTheme ? ccColor3B{47, 44, 56} : ccColor3B{255, 170, 85};

    auto searchBGColor = useGeodeTheme ? ccColor3B{60, 60, 60} : ccColor3B{0, 0, 0};

    auto bottomLayer = CCLayerColor::create(mainColor);
    bottomLayer->setAnchorPoint({0.f, 0.f});
    bottomLayer->ignoreAnchorPointForPosition(false);
    bottomLayer->setContentSize({m_size.width - leftOffset - rightOffset, 25.f});
    bottomLayer->setPosition({leftOffset, bottomOffset});
    bottomLayer->setID("bottom-layer"_spr);
    bottomLayer->setZOrder(2);

    auto bottomShadow = CCSprite::createWithSpriteFrameName("d_gradient_c_01_001.png");
    bottomShadow->setOpacity(30);
    bottomShadow->setColor({0, 0, 0});
    bottomShadow->setAnchorPoint({0.f, 0.f});
    bottomShadow->setScaleY(0.075f);
    bottomShadow->setScaleX(bottomLayer->getContentWidth() / bottomShadow->getContentWidth());
    bottomShadow->setPositionY(bottomLayer->getContentHeight());
    bottomShadow->setID("shadow"_spr);

    bottomLayer->addChild(bottomShadow);

    m_mainLayer->addChild(bottomLayer);

    auto settingsArea = CCLayerColor::create(secondaryColor);
    settingsArea->setContentSize({settingsWidth, m_size.height - topOffset - bottomOffset - bottomLayer->getContentHeight()});
    settingsArea->setAnchorPoint({1.f, 1.f});
    settingsArea->ignoreAnchorPointForPosition(false);
    settingsArea->setPosition(m_size - CCSize{rightOffset, topOffset});
    settingsArea->setID("settings-layer"_spr);
    
    m_mainLayer->addChild(settingsArea);

    auto titleLayer = CCLayerColor::create({0, 0, 0, 100});
    titleLayer->setAnchorPoint({1.f, 1.f});
    titleLayer->ignoreAnchorPointForPosition(false);
    titleLayer->setContentSize({settingsWidth, 45.f});
    titleLayer->setPosition(settingsArea->getContentSize());
    titleLayer->setID("title-layer"_spr);
    titleLayer->setZOrder(2);

    auto searchShadow = CCSprite::createWithSpriteFrameName("d_gradient_c_01_001.png");
    searchShadow->setOpacity(30);
    searchShadow->setFlipY(true);
    searchShadow->setColor({0, 0, 0});
    searchShadow->setAnchorPoint({0.f, 1.f});
    searchShadow->setScaleY(0.075f);
    searchShadow->setScaleX(titleLayer->getContentWidth() / searchShadow->getContentWidth());
    searchShadow->setID("shadow"_spr);

    titleLayer->addChild(searchShadow);

    settingsArea->addChild(titleLayer);

    float searchPadding = 5.f;
    float searchScale = 0.55f;

    auto searchInput = geode::TextInput::create(titleLayer->getContentWidth() / searchScale - searchPadding * 2.f - 55.f, "Search");
    searchInput->setID("search-input"_spr);
    searchInput->setAnchorPoint({0.f, 0.f});
    searchInput->setScale(searchScale);
    searchInput->setTextAlign(TextInputAlign::Left);
    searchInput->setPosition({searchPadding, searchPadding});
    searchInput->getBGSprite()->setOpacity(45);
    searchInput->getBGSprite()->setColor(searchBGColor);
    searchInput->setCallback([this] (const std::string& str) {
        m_searchQuery = geode::utils::string::trim(str);
        loadSettingNodes(false);
    });

    titleLayer->addChild(searchInput);

    m_titleLabel = geode::Label::create("All Settings", "bigFont.fnt");
    m_titleLabel->setScale(0.5f);
    m_titleLabel->setAnchorPoint({0.f, 0.f});
    m_titleLabel->setLimitLabelWidth(titleLayer->getContentWidth() - 10.f, 0.4f, 0.1f);
    m_titleLabel->setPosition({6.f, searchInput->boundingBox().getMaxY() + 4.f});
    m_titleLabel->setID("title"_spr);
    titleLayer->addChild(m_titleLabel);

    auto clearBtn = geode::Button::createWithSpriteFrameName("GJ_longBtn07_001.png", [this, searchInput] (auto sender) {
        searchInput->setString("");
        searchInput->defocus();
        m_searchQuery = "";
        loadSettingNodes(false);
    });
    clearBtn->setScale(0.55f);
    clearBtn->setPosition({searchInput->boundingBox().getMaxX() + searchPadding + clearBtn->getScaledContentWidth() / 2.f, searchInput->boundingBox().getMidY()});
    clearBtn->setID("clear-button"_spr);
    
    titleLayer->addChild(clearBtn);
    
    m_settingScrollLayer = alpha::ui::AdvancedScrollLayer::create({settingsWidth, settingsArea->getContentHeight() - titleLayer->getScaledContentHeight()});
    m_settingScrollLayer->setAnchorPoint({1.f, 1.f});
    m_settingScrollLayer->setPosition(settingsArea->getContentSize() - CCSize{0, titleLayer->getScaledContentHeight()});
    m_settingScrollLayer->getContentLayer()->setLayout(SimpleColumnLayout::create()
        ->setMainAxisDirection(AxisDirection::TopToBottom)
        ->setMainAxisAlignment(MainAxisAlignment::End)
        ->setMainAxisScaling(AxisScaling::Fit)
        ->ignoreInvisibleChildren(false)
    );
    m_settingScrollLayer->setID("settings-scroll-layer"_spr);

    settingsArea->addChild(m_settingScrollLayer);

    auto settingScrollbar = alpha::ui::AdvancedScrollBar::create(m_settingScrollLayer, alpha::ui::ScrollOrientation::VERTICAL);
    auto style = alpha::ui::RoundedScrollStyle();
    style.m_track = [] {
        auto track = alpha::ui::RoundedScrollTrack::create();
        track->setClickColor({0, 0, 0, 0});
        track->setBackgroundColor({0, 0, 0, 0});
        return track;
    };

    settingScrollbar->setStyle(style);
    settingScrollbar->setPositionX(settingScrollbar->getPositionX() - 14.f);
    settingScrollbar->setContentWidth(10.f);
    settingScrollbar->setID("settings-scroll-bar"_spr);

    settingsArea->addChild(settingScrollbar);

    auto categoryArea = CCLayerColor::create(mainColor);
    categoryArea->setContentSize({m_size.width - settingsWidth - leftOffset - rightOffset, m_size.height - topOffset - bottomOffset});
    categoryArea->setAnchorPoint({0.f, 0.f});
    categoryArea->setZOrder(1);
    categoryArea->ignoreAnchorPointForPosition(false);
    categoryArea->setPosition({leftOffset, bottomOffset});
    categoryArea->setID("category-layer"_spr);
    
    auto catgeoryShadow = CCSprite::createWithSpriteFrameName("d_gradient_c_01_001.png");
    catgeoryShadow->setOpacity(30);
    catgeoryShadow->setRotation(90);
    catgeoryShadow->setColor({0, 0, 0});
    catgeoryShadow->setAnchorPoint({1.f, 0.f});
    catgeoryShadow->setScaleY(0.075f);
    catgeoryShadow->setScaleX(categoryArea->getScaledContentHeight() / catgeoryShadow->getContentWidth());
    catgeoryShadow->setPositionX(categoryArea->getContentWidth());
    catgeoryShadow->setID("shadow"_spr);

    categoryArea->addChild(catgeoryShadow);

    m_mainLayer->addChild(categoryArea);

    auto supportBtn = tinker::ui::SupportButton::create(categoryArea->getContentWidth());
    supportBtn->setPosition({0, bottomLayer->getContentHeight()});
    supportBtn->setZOrder(1);
    supportBtn->setID("support-button"_spr);

    categoryArea->addChild(supportBtn);

    auto catgeoryScrollBG = CCLayerColor::create({0, 0, 0, static_cast<GLubyte>(useGeodeTheme ? 150 : 127)});
    catgeoryScrollBG->setContentSize({categoryArea->getContentWidth(), categoryArea->getContentHeight() - 60.f - supportBtn->getContentHeight()});
    catgeoryScrollBG->setAnchorPoint({0.f, 0.f});
    catgeoryScrollBG->setPosition({0.f, bottomLayer->getContentHeight() + supportBtn->getContentHeight()});
    catgeoryScrollBG->ignoreAnchorPointForPosition(false);
    catgeoryScrollBG->setID("category-scroll-bg"_spr);

    auto catgeoryScrollBGShadow = CCSprite::createWithSpriteFrameName("d_gradient_c_01_001.png");
    catgeoryScrollBGShadow->setOpacity(30);
    catgeoryScrollBGShadow->setFlipY(true);
    catgeoryScrollBGShadow->setColor({0, 0, 0});
    catgeoryScrollBGShadow->setAnchorPoint({0.f, 1.f});
    catgeoryScrollBGShadow->setScaleY(0.075f);
    catgeoryScrollBGShadow->setScaleX(catgeoryScrollBG->getScaledContentWidth() / catgeoryScrollBGShadow->getContentWidth());
    catgeoryScrollBGShadow->setPositionY(catgeoryScrollBG->getContentHeight());
    catgeoryScrollBGShadow->setID("shadow"_spr);

    catgeoryScrollBG->addChild(catgeoryScrollBGShadow);

    categoryArea->addChild(catgeoryScrollBG);

    m_categoryScrollLayer = alpha::ui::AdvancedScrollLayer::create(catgeoryScrollBG->getContentSize());
    m_categoryScrollLayer->setAnchorPoint({0.f, 0.f});
    m_categoryScrollLayer->setPosition({0.f, 0.f});
    m_categoryScrollLayer->getContentLayer()->setLayout(SimpleColumnLayout::create()
        ->setMainAxisDirection(AxisDirection::TopToBottom)
        ->setMainAxisAlignment(MainAxisAlignment::End)
        ->setCrossAxisAlignment(CrossAxisAlignment::Start)
        ->setMainAxisScaling(AxisScaling::Fit)
        ->ignoreInvisibleChildren(false)
        ->setPadding({12.f, 5.f, 0.f, 5.f})
    );
    #ifdef GEODE_IS_MOBILE
    m_categoryScrollLayer->setScrollDelta(1.5f);
    #else
    m_categoryScrollLayer->setScrollDelta(1.f);
    #endif
    m_categoryScrollLayer->setID("catgeory-scroll-layer"_spr);

    createCategoryButton("All Settings", "");

    for (auto& category : SettingsCache::get()->getCategoryList()) {
        createCategoryButton(category->name, category->id);
    }

    m_categoryScrollLayer->getContentLayer()->updateLayout();

    catgeoryScrollBG->addChild(m_categoryScrollLayer);

    auto categoryScrollbar = alpha::ui::AdvancedScrollBar::create(m_categoryScrollLayer, alpha::ui::ScrollOrientation::VERTICAL);
    categoryScrollbar->setStyle(style);
    categoryScrollbar->setPositionX(categoryScrollbar->getPositionX() - 14.f);
    categoryScrollbar->setContentWidth(10.f);
    categoryScrollbar->setID("catgeory-scroll-bar"_spr);

    catgeoryScrollBG->addChild(categoryScrollbar);

    auto titleLabel = geode::Label::create("Tinker Settings", "goldFont.fnt");
    titleLabel->setID("settings-title-label"_spr);
    titleLabel->setAnchorPoint({0.5f, 1.f});
    titleLabel->setScale(0.5f);
    titleLabel->setPosition({categoryArea->getContentWidth() / 2.f, categoryArea->getContentHeight() - 15.f});

    categoryArea->addChild(titleLabel);

    auto applyBtnSpr = ButtonSprite::create("Apply", 50, 0, 1.f, true, "goldFont.fnt", useGeodeTheme ? "geode.loader/GE_button_05.png" : "GJ_button_01.png", 30.f);
    applyBtnSpr->setCascadeColorEnabled(true);
    applyBtnSpr->setCascadeOpacityEnabled(true);
    applyBtnSpr->setScale(0.6f);

    auto applyBtn = geode::Button::createWithNode(applyBtnSpr, [this] (auto sender) {
        applyUncommitted();
    });

    applyBtn->setID("apply-button"_spr);
    applyBtn->setPosition(bottomLayer->getContentSize() / 2.f + CCPoint{0.f, 0.6f});
    applyBtn->setEnabled(false);
    applyBtn->setCascadeColorEnabled(true);
    applyBtn->setCascadeOpacityEnabled(true);
    applyBtn->setColor({155, 155, 155});
    applyBtn->setOpacity(155);

    bottomLayer->addChild(applyBtn);

    auto resetBtnSpr = ButtonSprite::create("Reset", 50, 0, 1.f, true, "goldFont.fnt", useGeodeTheme ? "geode.loader/GE_button_05.png" : "GJ_button_01.png", 30.f);
    resetBtnSpr->setScale(0.5f);
    
    auto resetBtn = geode::Button::createWithNode(resetBtnSpr, [this] (auto sender) {
        createQuickPopup(
            "Reset All",
            "Are you sure you want to <cr>reset</c> ALL settings "
            "to <cy>default</c>?",
            "Cancel", "Reset",
            [this](auto, bool btn2) {
                if (btn2) {
                    for (auto& settingNode : m_settingNodes) {
                        auto setting = settingNode->getSetting();
                        if (!setting || setting->isDefaultValue()) continue;
                        settingNode->resetToDefault();
                    }
                }
            }
        );
    });

    resetBtn->setID("reset-button"_spr);
    resetBtn->setPosition({5.f + resetBtn->getScaledContentWidth() / 2.f, applyBtn->getPositionY()});

    bottomLayer->addChild(resetBtn);

    resetBtnSpr->setScale(0.5f);
    
    auto folderSpr = CCSprite::createWithSpriteFrameName("folderIcon_001.png");
    auto folderSprSub = CCSprite::createWithSpriteFrameName("geode.loader/save.png");
    folderSprSub->setColor({0, 0, 0});
    folderSprSub->setOpacity(155);
    folderSprSub->setScale(0.55f);

    folderSpr->addChildAtPosition(folderSprSub, Anchor::Center, ccp(0, -3));
    auto savedBtnSpr = IconButtonSprite::create(useGeodeTheme ? "geode.loader/GE_button_05.png" : "GJ_button_01.png", folderSpr, "", "bigFont.fnt");
    savedBtnSpr->setScale(0.45f);
    savedBtnSpr->getIcon()->setScale(savedBtnSpr->getIcon()->getScale() * 1.4f);

    auto savedBtn = geode::Button::createWithNode(savedBtnSpr, [this] (auto sender) {
        file::openFolder(Mod::get()->getSaveDir());
    });
    savedBtn->setID("saved-button"_spr);
    savedBtn->setPosition({bottomLayer->getContentWidth() - 5.f - savedBtn->getScaledContentWidth() / 2.f, applyBtn->getPositionY()});

    bottomLayer->addChild(savedBtn);

    auto borderLeft = tinker::ui::PopupBorder::create(topLeftColor, 255, bottomColor, 255);
    borderLeft->setContentSize({m_size.width - settingsWidth - rightOffset, m_size.height});
    borderLeft->hideRight(true);
    borderLeft->setZOrder(10001);
    borderLeft->setAnchorPoint({0.f, 0.f});
    borderLeft->setPosition({0.f, 0.f});
    borderLeft->setID("categories-border"_spr);

    m_mainLayer->addChild(borderLeft);

    auto borderRight = tinker::ui::PopupBorder::create(topRightColor, 255, bottomColor, 255);
    borderRight->setContentSize({settingsWidth + rightOffset, m_size.height});
    borderRight->hideLeft(true);
    borderRight->setZOrder(borderLeft->getZOrder());
    borderRight->setAnchorPoint({1.f, 0.f});
    borderRight->setPosition({m_size.width, 0});
    borderRight->setID("settings-border"_spr);

    m_mainLayer->addChild(borderRight);

    m_buttonMenu->setZOrder(borderLeft->getZOrder() + 1);
    m_buttonMenu->setID("main-menu");
    m_closeBtn->setID("close-button"_spr);
    m_mainLayer->setID("main-layer");

    addEventListener(SettingNodeValueChangeEventV3(), [this, applyBtn] (std::string_view modID, std::string_view key, SettingNodeV3* node, bool committed)  {
        if (modID != Mod::get()->getID() || committed) return;

        bool uncommitted = hasUncommitted();
        applyBtn->setEnabled(uncommitted);

        applyBtn->setColor(uncommitted ? ccColor3B{255, 255, 255} : ccColor3B{155, 155, 155});
        applyBtn->setOpacity(uncommitted ? 255 : 155);

        if (auto title = typeinfo_cast<TitleSettingNodeV3*>(node)) {
            auto toggler = node->getButtonMenu()->getChildByType<CCMenuItemToggler>(0);
            if (!toggler) return;

            m_categoryCollapsed[std::string(key)] = toggler->isToggled();

            loadSettingNodes();
        }
    });

    addEventListener(SliderStateChanged(), [this] (bool started) {
        m_settingScrollLayer->setDraggingEnabled(!started);
    });

    loadSettings();
    loadSettingNodes();

    return true;
}

// there's some weird geode::Button bug with my scroll layers, so using regular buttons for now
CCMenu* SettingsPopup::createCategoryButton(ZStringView name, ZStringView id) {
    auto label = CCLabelBMFont::create(name.c_str(), "bigFont.fnt");
    label->limitLabelWidth(m_categoryScrollLayer->getContentWidth() - 30.f, 0.3f, 0.1f);
    
    auto btn = CCMenuItemExt::createSpriteExtra(label, [this, id, name] (auto sender) {
        m_category = id;
        switchCategory();
        sender->setColor({255, 255, 0});
        m_titleLabel->setString(name.c_str());
    });
    btn->setCascadeColorEnabled(true);
    btn->setCascadeOpacityEnabled(true);
    if (id.empty()) {
        btn->setColor({255, 255, 0});
    }
    btn->setID(fmt::format("category-button"_spr));

    label->setAnchorPoint({0.f, 0.5f});
    btn->setContentSize(label->getScaledContentSize() + CCSize{0.f, 10.f});
    label->setPosition({0.f, btn->getContentHeight() / 2.f});
    label->setID(fmt::format("category-label"_spr));

    m_categoryButtons.push_back(btn);

    auto menu = CCMenu::create();
    menu->setID(fmt::format("{}-category"_spr, id));
    menu->setContentSize(btn->getContentSize());
    menu->addChild(btn);
    btn->setPosition(menu->getContentSize() / 2.f);

    m_categoryScrollLayer->getContentLayer()->addChild(menu);

    return menu;
}

void SettingsPopup::switchCategory() {
    for (auto btn : m_categoryButtons) {
        btn->setColor({255, 255, 255});
    }
    loadSettingNodes();
}

void SettingsPopup::applyUncommitted() {
    bool hasUncommitted = false;
    for (const auto& node : m_settingNodes) {
        if (node->settingWasChanged()) {
            hasUncommitted = true;
            node->commit();
        }
    }
    if (hasUncommitted) {
        for (const auto& node : m_settingNodes) {
            node->updateState();
        }
    }
}

void SettingsPopup::loadSettings() {
    auto& settings = SettingsCache::get()->getSettingsList();

    for (const auto& settingInfo : settings) {
        auto setting = Mod::get()->getSetting(settingInfo->id);
        if (!setting) continue;

        auto node = tinker::settings::SettingNodeRegistry::get()->create(settingInfo->type, m_settingScrollLayer->getContentWidth(), setting);
        if (!node) continue;

        node->setID(fmt::format("{}-setting-node"_spr, setting->getKey()));

        m_settingNodes.push_back(node);
        m_settingNodeMap[node] = settingInfo;
    }
}

void SettingsPopup::loadSettingNodes(bool retainPosition) {
    auto scroll = m_settingScrollLayer->getScrollPoint().y;
    m_settingScrollLayer->getContentLayer()->removeAllChildren();
    runAction(CallFuncExt::create([this, retainPosition, scroll] {
        bool even = false;

        std::unordered_map<SettingsCache::SettingCategory*, std::set<std::shared_ptr<SettingsCache::SettingInfo>>> m_resultsForQuery;
        for (const auto& category : SettingsCache::get()->getCategoryList()) {
            m_resultsForQuery[category] = category->settingsForSearch(m_searchQuery);
        }
        
        for (const auto& node : m_settingNodes) {
            auto& info = m_settingNodeMap[node];
            if (m_category.empty()) {
                if (m_categoryCollapsed[info->category->id] && info->type != "title") continue;
            }
            else {
                if (info->category->id != m_category || info->type == "title") continue;
            }

            if (!m_resultsForQuery[info->category].contains(info)) continue;

            node->setOpacity(even ? 10 : 50);
            m_settingScrollLayer->addChild(node);
            even = !even;
        }

        m_settingScrollLayer->getContentLayer()->updateLayout();
        if (retainPosition) {
            m_settingScrollLayer->setScrollY(scroll);
        }
        m_settingScrollLayer->forceCull();
    }));
}

bool SettingsPopup::hasUncommitted() {
    for (const auto& node : m_settingNodes) {
        if (node->settingWasChanged()) {
            return true;
        }
    }
    return false;
}

void SettingsPopup::onClose(CCObject* sender) {
    if (hasUncommitted()) {
        createQuickPopup(
            "Unsaved Changes",
            "You have <cr>unsaved changes</c>! Are you sure you "
            "want to exit?",
            "Cancel", "Discard",
            [this](FLAlertLayer*, bool btn2) {
                if (btn2) {
                    Popup::onClose(nullptr);
                }
            }
        );
        return;
    }
    Popup::onClose(sender);
}

}

class $modify(SettingsSliderTouchLogic, SliderTouchLogic) {

    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
        auto ret = SliderTouchLogic::ccTouchBegan(touch, event);
        if (ret) {
            SliderStateChanged().send(true);
        }
        return ret;
    }

    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
        SliderStateChanged().send(false);
        SliderTouchLogic::ccTouchEnded(touch, event);
    }
};

class $nodeModify(MyModPopup, ModPopup) {

	void modify() {
        if (getID() != "geode.loader/popup-alphalaneous.tinker") return;
		auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(getChildByIDRecursive("settings-button"));
        if (btn) {
            tinker::utils::hijackButton(btn, [] (std::function<void(CCObject* sender)> orig, CCObject* sender) {
        	    auto geode = tinker::utils::getMod<"geode.loader">();
                auto theme = geode->getSettingValue<std::string>("used-theme");
                
                bool geodeTheme = theme != "Geometry Dash";
                tinker::ui::SettingsPopup::create(geodeTheme)->show();
            });
        }
	}
};