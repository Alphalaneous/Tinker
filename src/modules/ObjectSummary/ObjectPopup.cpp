#include "ObjectPopup.hpp"
#include "ObjectList.hpp"

using namespace tinker::ui;

bool ObjectPopup::init(LevelEditorLayer* levelEditorLayer) {
  
    m_editorLayer = levelEditorLayer;

    if (!Popup::init(400.f, 275.f, "GJ_square01.png")) return false;

    setCloseButtonSpr(
        CircleButtonSprite::createWithSpriteFrameName(
            "geode.loader/close.png", .85f,
            CircleBaseColor::Green
        )
    );

    setTitle("Object Summary");
    m_title->limitLabelWidth(m_mainLayer->getContentSize().width - 50.f, 0.7f, 0.1f);
    m_title->setID("title"_spr);

    m_uniqueLabel = CCLabelBMFont::create("0 unique objects", "bigFont.fnt");
    m_uniqueLabel->setScale(0.25f);
    m_uniqueLabel->setOpacity(127);
    m_uniqueLabel->setAnchorPoint({1.f, 0.5f});
    m_uniqueLabel->setID("unique-label"_spr);

    m_mainLayer->addChildAtPosition(
        m_uniqueLabel,
        Anchor::BottomRight,
        { -17.0f, 20.5f }
    );

    generateList(m_sortOptions);

    m_sortButtons = CCMenu::create();
    m_sortButtons->setContentSize({30.f, m_mainLayer->getContentHeight()});
    m_sortButtons->setScale(0.7f);
    m_sortButtons->setID("sort-menu"_spr);

    auto layout = ColumnLayout::create();
    layout->setAxisAlignment(AxisAlignment::End);
    layout->setAxisReverse(true);
    layout->setAutoScale(true);
    m_sortButtons->setLayout(layout);

    auto sortToggler = createToggler("GJ_sortIcon_001.png", menu_selector(ObjectPopup::onSort), false, 1);
    sortToggler->setID("sort-toggler"_spr);

    m_sortButtons->addChild(sortToggler);

    auto emptyDivider = CCNode::create();
    emptyDivider->setContentSize({1.f, 5.f});
    emptyDivider->setID("divider"_spr);

    m_sortButtons->addChild(emptyDivider);

    createFilterToggler("square_01_001.png", 0.85f, 0, "solids-toggler"_spr);
    createFilterToggler("spike_01_001.png", 0.85f, 1, "hazards-toggler"_spr);
    createFilterToggler("d_02_chain_02_001.png", 0.75f, 2, "decoration-toggler"_spr);
    createFilterToggler("portal_04_front_001.png", 1.f, 3, "portals-toggler"_spr);
    createFilterToggler("ring_01_001.png", 0.75f, 4, "pads-and-orbs-toggler"_spr);
    createFilterToggler("edit_eSpawnBtn_001.png", 0.85f, 5, "triggers-toggler"_spr);

    m_sortButtons->updateLayout();

    m_mainLayer->addChildAtPosition(
        m_sortButtons,
        Anchor::Left,
        { 24.f, -5.f }
    );

    m_mainLayer->setID("main-layer"_spr);

    auto hideToggle = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(ObjectPopup::onShowHidden), 1.f);
    hideToggle->toggle(true);
    hideToggle->setID("hide-toggler"_spr);

    auto hideMenu = CCMenu::create();
    hideMenu->setContentSize({35.f, 35.f});
    hideMenu->setScale(0.5f);
    hideMenu->setID("hide-menu"_spr);

    auto layout2 = ColumnLayout::create();
    hideMenu->setLayout(layout2);

    hideMenu->addChild(hideToggle);
    hideMenu->updateLayout();

    m_mainLayer->addChildAtPosition(
        hideMenu,
        Anchor::BottomLeft,
        { 24.f, 20.f }
    );

    auto countHiddenLabel = CCLabelBMFont::create("Count Hidden", "bigFont.fnt");
    countHiddenLabel->setScale(0.25f);
    countHiddenLabel->setID("count-hidden-label"_spr);
    countHiddenLabel->setAnchorPoint({0.f, 0.5f});
    m_mainLayer->addChildAtPosition(
        countHiddenLabel,
        Anchor::BottomLeft,
        { 36.f, 20.5f }
    );

    return true;
}

void ObjectPopup::createFilterToggler(const std::string& spr, float scale, int tag, ZStringView id) {
    auto toggler = createToggler(spr, menu_selector(ObjectPopup::onFilter), true, scale);
    toggler->setTag(tag);
    toggler->setID(id);

    m_togglers.push_back(toggler);
    m_sortButtons->addChild(toggler);
}

CCMenuItemToggler* ObjectPopup::createToggler(const std::string& spr, cocos2d::SEL_MenuHandler selector, bool alt, float scale) {

    auto onSpr = CCSprite::createWithSpriteFrameName(spr.c_str());
    auto offSpr = CCSprite::createWithSpriteFrameName(spr.c_str());

    std::string onSprStr;
    std::string offSprStr;

    if (!alt) {
        onSprStr = "GJ_button_01.png";
        offSprStr = "GJ_button_03.png";
    }
    else {
        onSprStr = "GJ_button_05.png";
        offSprStr = "GJ_button_02.png";
    }

    auto on = ButtonSprite::create(onSpr, 30, true, 30.f, onSprStr.c_str(), scale);
    auto off = ButtonSprite::create(offSpr, 30, true, 30.f, offSprStr.c_str(), scale);

    onSpr->setPosition({on->getContentSize().width / 2.f, on->getContentSize().height / 2.f});
    offSpr->setPosition({off->getContentSize().width / 2.f, off->getContentSize().height / 2.f});

    return CCMenuItemToggler::create(on, off, this, selector);
}

void ObjectPopup::onFilter(CCObject* sender) {
    auto toggler = static_cast<CCMenuItemToggler*>(sender);
    bool state = toggler->isOn();

    if (state) {
        m_sortOptions.sortType = SummarySortType::None;
    }
    else {
        m_sortOptions.sortType = static_cast<SummarySortType>(sender->getTag());
    }

    for (auto toggle : m_togglers) {
        toggle->toggle(false);
    }

    toggler->toggle(state);
    generateList(m_sortOptions);
}

void ObjectPopup::onSort(CCObject* sender) {
    auto toggler = static_cast<CCMenuItemToggler*>(sender);
    m_sortOptions.isAscending = !toggler->isOn();
    generateList(m_sortOptions);
}

void ObjectPopup::onShowHidden(CCObject* sender) {
    auto toggler = static_cast<CCMenuItemToggler*>(sender);
    m_sortOptions.showHidden = !toggler->isOn();
    generateList(m_sortOptions);
}

void ObjectPopup::generateList(const SortOptions& sortOptions) {
    std::map<int, int> objectCounts;

    for (auto obj : m_editorLayer->m_objects->asExt<GameObject>()){
        if (!sortOptions.showHidden && obj->m_isHide){
            continue;
        }
        if (sortOptions.sortType == SummarySortType::Decoration && !(obj->m_isDecoration || obj->m_isDecoration2)){
            continue;
        }
        objectCounts[obj->m_objectID]++;
    }

    std::map<int, bool> objectTotals;

    for (auto obj : m_editorLayer->m_objects->asExt<GameObject>()){
        objectTotals[obj->m_objectID] = true;
    }

    if (m_objectList) m_objectList->removeFromParent();
    
    m_objectList = ObjectList::create(objectCounts, sortOptions, {m_mainLayer->getContentSize().width- 60.f, m_mainLayer->getContentSize().height - 75.f});
    m_objectList->setID("object-list"_spr);
    
    m_mainLayer->addChildAtPosition(
        m_objectList,
        Anchor::Center,
        {15.f, -5.f }
    );

    std::string text;

    if (objectTotals.size() == 1){
        text = fmt::format("{} unique object", objectTotals.size());
    }
    else {
        text = fmt::format("{} unique objects", objectTotals.size());
    }

    m_uniqueLabel->setString(text.c_str());
}

ObjectPopup* ObjectPopup::create(LevelEditorLayer* levelEditorLayer) {
    auto ret = new ObjectPopup();
    if (ret->init(levelEditorLayer)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}