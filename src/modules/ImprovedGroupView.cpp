#include "ImprovedGroupView.hpp"
#include "../../include/ImprovedGroupView.hpp"
#include <spaghettdev.named-editor-groups/api/NIDManager.hpp>

bool ImprovedGroupView::onToggled(bool state) {
    return true;
}

bool ImprovedGroupView::onSettingChanged(std::string_view key, const matjson::Value& value) {
    return true;
}

bool IGVSetGroupIDLayer::init(GameObject* obj, cocos2d::CCArray* objs) {
    if (!SetGroupIDLayer::init(obj, objs)) return false;

    if (auto node = m_mainLayer->getChildByID("groups-list-menu")) {
        auto replacementMenu = CCMenu::create();
        replacementMenu->setPosition(node->getPosition());
        replacementMenu->setContentSize(node->getContentSize());
        replacementMenu->setScaleX(node->getScaleX());
        replacementMenu->setScaleY(node->getScaleY());
        replacementMenu->setAnchorPoint(node->getAnchorPoint());
        replacementMenu->setID("z-layer-menu"_spr);

        m_mainLayer->addChild(replacementMenu);

        node->setVisible(false);
        
        if (auto btn = node->getChildByID("z-layer-decrement-button")) {
            btn->removeFromParentAndCleanup(false);
            replacementMenu->addChild(btn);
        }
        if (auto btn = node->getChildByID("z-layer-increment-button")) {
            btn->removeFromParentAndCleanup(false);
            replacementMenu->addChild(btn);
        }
        if (auto btn = node->getChildByID("settings-button")) {
            btn->removeFromParentAndCleanup(false);
            replacementMenu->addChild(btn);
        }
    }

    if (auto node = m_mainLayer->getChildByID("add-group-id-buttons-menu")) {
        if (auto idBtn = typeinfo_cast<CCMenuItemSpriteExtra*>(node->getChildByID("add-group-id-button"))) {
            tinker::utils::hijackButton(idBtn, [this] (auto orig, auto sender) {
                orig(sender);
                regenerateGroupView();
            });
        }

        if (auto parentBtn = typeinfo_cast<CCMenuItemSpriteExtra*>(node->getChildByID("add-group-parent-button"))) {
            tinker::utils::hijackButton(parentBtn, [this] (auto orig, auto sender) {
                orig(sender);
                regenerateGroupView();
            });
        }
    }

    if (tinker::utils::getMod<"spaghettdev.named-editor-groups">()) {
        schedule(schedule_selector(IGVSetGroupIDLayer::checkNamedIDs));
    }
    
    regenerateGroupView();

    m_fields->m_listener = tinker::api::improved_group_view::UpdateGroupView().listen([this] {
        regenerateGroupView();
        return ListenerResult::Propagate;
    });

    return true;
}

void IGVSetGroupIDLayer::checkNamedIDs(float dt) {
    auto fields = m_fields.self();
    auto namedIDsRes = NIDManager::getNamedIDs(NID::GROUP);
    if (!namedIDsRes) return;

    auto namedIDs = namedIDsRes.unwrap();
    if (namedIDs != fields->m_namedIDs) {
        fields->m_namedIDs = namedIDs;
        regenerateGroupView();
    }
}

void IGVSetGroupIDLayer::onRemoveFromGroup2(CCObject* obj) {
    m_fields->m_lastRemoved = obj->getTag();
    SetGroupIDLayer::onRemoveFromGroup(obj);
    regenerateGroupView();
}

void IGVSetGroupIDLayer::regenerateGroupView() {
    auto fields = m_fields.self();

    if (fields->m_scrollLayer) {
        fields->m_scrollLayer->removeFromParent();
    }

    if (fields->m_scrollBar) {
        fields->m_scrollBar->removeFromParent();
    }

    std::vector<ImprovedGroupView::GroupData> groupData;
    std::map<int, int> allGroups;
    std::map<int, int> allParentGroups;

    if (!m_targetObjects || m_targetObjects->count() == 0) {
        if (m_targetObject) {
            groupData.push_back(parseObjGroups(m_targetObject));
        }
    }
    else {
        for (auto obj : CCArrayExt<GameObject*>(m_targetObjects)) {
            groupData.push_back(parseObjGroups(obj));
        }
    }

    for (const auto& data : groupData) {
        for (int group : data.groups) {
            allGroups[group]++;
        }
        for (int group : data.parentGroups) {
            allParentGroups[group]++;
        }
    }

    allGroups.erase(0);
    allParentGroups.erase(0);

    if (allParentGroups.count(fields->m_lastRemoved)) {
        allParentGroups.erase(fields->m_lastRemoved);
    }
    else {
        allGroups.erase(fields->m_lastRemoved);
    }

    auto menuContainer = CCNode::create();
    auto groupsMenu = CCMenu::create();

    auto layout = RowLayout::create();
    layout->setGap(12);
    layout->setAutoScale(false);
    layout->setGrowCrossAxis(true);
    layout->setCrossAxisOverflow(true);
    if (ImprovedGroupView::getSetting<bool, "left-align">()) {
        layout->setAxisAlignment(AxisAlignment::Start);
    }

    groupsMenu->setLayout(layout);

    fields->m_lastRemoved = 0;
    bool isNamed = tinker::utils::getMod<"spaghettdev.named-editor-groups">();

    for (const auto& [k, v] : allGroups) {
        bool isParent = allParentGroups.count(k);
        bool isAlwaysPresent = v == groupData.size();

        std::string texture = "GJ_button_04.png";

        if (!isAlwaysPresent) texture = "GJ_button_05.png";
        if (isParent) texture = "GJ_button_03.png";

        std::string name = "";
        if (isNamed) {
            name = NIDManager::getNameForID(NID::GROUP, k).unwrapOrDefault();
        }

        auto bspr = ButtonSprite::create(fmt::format("{}", k).c_str(), 30, true, "goldFont.fnt", texture.c_str(), 20, 0.5);
        bspr->setID("background-sprite"_spr);

        float width = 46;

        auto nameLabel = CCLabelBMFont::create(name.c_str(), "bigFont.fnt");
        nameLabel->setScale(0.5);

        if (!name.empty()) {
            bspr->m_label->setAnchorPoint({0.f, 0.5f});
            bspr->m_label->setPositionX(10);
            auto idLabelSize = bspr->m_label->getScaledContentSize();
            auto idLabelPos = bspr->m_label->getPosition();
            nameLabel->setAnchorPoint({0.f, 0.5f});
            nameLabel->limitLabelWidth(70.f, .5, .1);
            width = nameLabel->getScaledContentWidth() + 25 + bspr->m_label->getScaledContentWidth();
            nameLabel->setPosition({bspr->m_label->getPositionX() + bspr->m_label->getScaledContentWidth() + 5.f, bspr->m_label->getPositionY()});
            bspr->addChild(nameLabel);

            auto background = CCSprite::create("square02b_001.png");
            background->setScaleX(idLabelSize.width / background->getScaledContentWidth() + .05f);
            background->setScaleY(idLabelSize.height / background->getScaledContentHeight() - .02f);
            background->setColor({0, 0, 0});
            background->setOpacity(100);
            background->setPosition({idLabelPos.x + idLabelSize.width/2, idLabelPos.y - 1.5f });
            background->setID("name-background"_spr);
            bspr->addChild(background);
        }

        bspr->m_BGSprite->setContentSize({width, bspr->m_BGSprite->getContentHeight()});
        bspr->setContentSize(bspr->m_BGSprite->getScaledContentSize());
        bspr->m_BGSprite->setPosition(bspr->getContentSize()/2);

        auto button = CCMenuItemSpriteExtra::create(bspr, this, menu_selector(IGVSetGroupIDLayer::onRemoveFromGroup2));
        button->setID(fmt::format("group-{}-button"_spr, k));
        button->setTag(k);
        
        groupsMenu->addChild(button);
    }
    CCSize contentSize;

    if (groupsMenu->getChildrenCount() <= 10) {
        groupsMenu->setScale(1.f);
        contentSize = CCSize{278, 67};
    }
    else {
        groupsMenu->setScale(0.85f);
        contentSize = CCSize{395, 67};
    }

    float padding = 7.5;

    groupsMenu->setContentSize(contentSize);
    groupsMenu->setPosition({360/2.f, padding});
    groupsMenu->setAnchorPoint({0.5, 0});
    groupsMenu->updateLayout();
    groupsMenu->setID("groups-menu"_spr);

    menuContainer->setContentSize({360, groupsMenu->getScaledContentSize().height + padding * 2});
    menuContainer->setAnchorPoint({0.5, 0});
    menuContainer->setPosition({360/2.f, 0});
    menuContainer->addChild(groupsMenu);
    menuContainer->setID("menu-container"_spr);

    auto winSize = CCDirector::get()->getWinSize();

    fields->m_scrollLayer = AdvancedScrollLayer::create({360, menuContainer->getScaledContentSize().height});

    fields->m_scrollLayer->setContentSize({360, 68});
    fields->m_scrollLayer->setPosition({winSize.width/2, winSize.height/2 - 16.8f});
    fields->m_scrollLayer->ignoreAnchorPointForPosition(false);
    fields->m_scrollLayer->addChild(menuContainer);
    fields->m_scrollLayer->setID("groups-list-scroll-layer"_spr);
    fields->m_scrollLayer->setTouchPriority(-504);
    fields->m_scrollLayer->setScrollDelta(1.5f);

    m_mainLayer->addChild(fields->m_scrollLayer);

    fields->m_scrollBar = AdvancedScrollBar::create(fields->m_scrollLayer, ScrollOrientation::VERTICAL);
    auto style = RoundedScrollStyle();
    style.m_track = [] () {
        auto track = RoundedScrollTrack::create();
        track->setClickColor({0, 0, 0, 0});
        track->setBackgroundColor({0, 0, 0, 0});
        return track;
    };

    fields->m_scrollBar->setStyle(style);
    fields->m_scrollBar->setContentSize({10, fields->m_scrollLayer->getContentHeight() - 10});
    fields->m_scrollBar->setPosition({fields->m_scrollLayer->getPositionX() + fields->m_scrollLayer->getContentWidth() / 2 - fields->m_scrollBar->getContentWidth() / 2 + 1, fields->m_scrollLayer->getPositionY()});
    fields->m_scrollBar->setZOrder(100);
    fields->m_scrollBar->setTouchPriority(fields->m_scrollLayer->getTouchPriority() - 10);

    m_mainLayer->addChild(fields->m_scrollBar);

    if (groupsMenu->getScaledContentHeight() <= 67) {
        fields->m_scrollLayer->setVerticalScroll(false);
        fields->m_scrollLayer->setHorizontalScrollWheel(false);
        fields->m_scrollBar->setVisible(false);
    }

    if (fields->m_groupCountLabel) fields->m_groupCountLabel->removeFromParent();

    fields->m_groupCountLabel = CCLabelBMFont::create(fmt::format("Groups: {}", allGroups.size()).c_str(), "chatFont.fnt");

    if (auto zLayerLabel = m_mainLayer->getChildByID("z-layer-label")) {
        if (auto groupsBG = m_mainLayer->getChildByID("groups-bg")) {
            auto labelPos = zLayerLabel->getPosition();
            auto groupsBGSize = groupsBG->getContentSize();
            fields->m_groupCountLabel->setPosition({labelPos.x - groupsBGSize.width/2, labelPos.y + 6});
        }
    }
    fields->m_groupCountLabel->setID("group-count-label"_spr);
    fields->m_groupCountLabel->setAnchorPoint({0, 0.5});
    fields->m_groupCountLabel->setColor({0, 0, 0});
    fields->m_groupCountLabel->setOpacity(200);
    fields->m_groupCountLabel->setScale(0.5f);
    m_mainLayer->addChild(fields->m_groupCountLabel);
}

ImprovedGroupView::GroupData IGVSetGroupIDLayer::parseObjGroups(GameObject* obj) {
    auto lel = LevelEditorLayer::get();

    int uuid = obj->m_uniqueID;
    std::vector<int> parents;

    for (const auto& [k, v] : CCDictionaryExt<int, CCArray*>(lel->m_parentGroupIDs)) {
        if (k != uuid) continue;
        for (auto val : CCArrayExt<CCInteger*>(v)) {
            parents.push_back(val->getValue());
        }
    }

    std::vector<int> groups;

    if (obj->m_groups) {
        groups = std::vector<int>{obj->m_groups->begin(), obj->m_groups->end()};
    }

    return ImprovedGroupView::GroupData{groups, parents, obj};
}

void IGVSetupSpawnPopup::addRemap(int oldID, int newID) {
    SetupSpawnPopup::addRemap(oldID, newID);
    m_fields->m_needsUpdate = true;
}

void IGVSetupSpawnPopup::onDeleteRemap(cocos2d::CCObject* sender) {
    SetupSpawnPopup::onDeleteRemap(sender);
    m_fields->m_needsUpdate = true;
}

void IGVSetupSpawnPopup::queueUpdateButtons() {
    SetupSpawnPopup::queueUpdateButtons();
    m_fields->m_needsUpdate = true;
}

bool IGVSetupSpawnPopup::init(EffectGameObject* object, cocos2d::CCArray* objects) {
    if (!SetupSpawnPopup::init(object, objects)) return false;

    if (tinker::utils::getMod<"spaghettdev.named-editor-groups">()) {
        schedule(schedule_selector(IGVSetupSpawnPopup::fixNamedEditorGroups));
    }

    return true;
}

void IGVSetupSpawnPopup::fixNamedEditorGroups(float dt) {
    auto fields = m_fields.self();
    if (fields->m_lastPage == m_page) return;
    fields->m_lastPage = m_page;

    if (m_page == 1) {
        if (tinker::utils::getMod<"spaghettdev.named-editor-groups">()) {
            runAction(CallFuncExt::create([this] {
                queueUpdateButtons();
            }));
        } 
    }
}

void IGVSetupSpawnPopup::updateRemapButtons(float dt) {
    auto fields = m_fields.self();

    if (!fields->m_needsUpdate) return;
    fields->m_needsUpdate = false;

    m_isBusy = false;

    for (auto btn : CCArrayExt<CCNode, false>(m_remapButtons)) {
        btn->removeFromParent();
    }

    auto namedEditorGroupsMenu = m_buttonMenu->getChildByID("spaghettdev.named-editor-groups/remaps-list-menu");
    if (namedEditorGroupsMenu) {
        namedEditorGroupsMenu->setVisible(false);
    }

    m_remapButtons->removeAllObjects();
    m_remapGroups.clear();

    float currentScroll = 0;

    if (fields->m_scrollLayer) {
        currentScroll = fields->m_scrollLayer->getScrollPoint().y;
        fields->m_scrollLayer->removeFromParent();
        removeObjectFromPage(fields->m_scrollLayer, 1);
    }

    if (fields->m_scrollBar) {
        fields->m_scrollBar->removeFromParent();
        removeObjectFromPage(fields->m_scrollBar, 1);
    }

    if (fields->m_groupCountLabel) {
        fields->m_groupCountLabel->removeFromParent();
        removeObjectFromPage(fields->m_groupCountLabel, 1);
    }

    auto winSize = CCDirector::get()->getWinSize();

    auto groupsMenu = CCMenu::create();

    auto objects = m_gameObject ? CCArray::createWithObject(m_gameObject) : m_gameObjects;

    struct RemapEntry {
        int groupID;
        int remapID;

        bool operator<(const RemapEntry& rhs) const {
            return std::tie(groupID, remapID) < std::tie(rhs.groupID, rhs.remapID);
        }
    };

    std::map<RemapEntry, int> remapCounts;

    for (auto obj : CCArrayExt<GameObject*>(objects)) {
        for (const auto& pair : static_cast<SpawnTriggerGameObject*>(obj)->m_remapObjects) {
            RemapEntry entry {pair.m_groupID, pair.m_chance};
            remapCounts[entry]++;
        }
    }

    int index = 0;

    for (const auto& [entry, count] : remapCounts) {
        auto sprite = entry.groupID == m_remapOriginalID && entry.remapID == m_remapNewID ? "GJ_button_03.png" : (count == objects->count() ? "GJ_button_04.png" : "GJ_button_05.png");

        auto buttonSprite = ButtonSprite::create(
            fmt::format("{}\n{}", entry.groupID, entry.remapID).c_str(),
            remapCounts.size() > 10 ? 15 : 40,
            0,
            0.35f,
            true,
            "bigFont.fnt",
            sprite,
            30.f
        );

        auto button = CCMenuItemSpriteExtra::create(
            buttonSprite,
            this,
            menu_selector(SetupSpawnPopup::onSelectRemap)
        );

        button->setTag(index);

        groupsMenu->addChild(button);

        m_remapButtons->addObject(button);

        m_remapGroups.emplace_back(entry.groupID);
        m_remapGroups.emplace_back(entry.remapID);

        index++;
    }

    auto layout = RowLayout::create();
    layout->setGap(10);
    layout->setAutoScale(false);
    layout->setGrowCrossAxis(true);
    layout->setCrossAxisOverflow(true);
    layout->setPadding({20, 10, 20, 10});
    
    if (ImprovedGroupView::getSetting<bool, "left-align">()) {
        layout->setAxisAlignment(AxisAlignment::Start);
    }

    groupsMenu->setLayout(layout);

    groupsMenu->setContentSize({360, 90});
    groupsMenu->setPosition({360/2.f, 0});
    groupsMenu->setAnchorPoint({0.5, 0});
    groupsMenu->updateLayout();
    groupsMenu->setID("groups-menu"_spr);

    if (groupsMenu->getScaledContentHeight() > 90) {
        layout->setPadding({20, 5, 20, 5});
        groupsMenu->updateLayout();
    }

    fields->m_scrollLayer = AdvancedScrollLayer::create({360, 90});

    fields->m_scrollLayer->setContentSize({360, 90});
    fields->m_scrollLayer->setPosition({winSize.width/2, winSize.height/2});
    fields->m_scrollLayer->ignoreAnchorPointForPosition(false);
    fields->m_scrollLayer->addChild(groupsMenu);
    fields->m_scrollLayer->setID("groups-list-scroll-layer"_spr);
    fields->m_scrollLayer->setTouchPriority(-504);
    fields->m_scrollLayer->setScrollDelta(1.5f);
    fields->m_scrollLayer->getContentLayer()->setLayout(SimpleRowLayout::create()->setCrossAxisScaling(AxisScaling::Grow)->setCrossAxisAlignment(CrossAxisAlignment::Start));

    m_mainLayer->addChild(fields->m_scrollLayer);

    fields->m_scrollLayer->getContentLayer()->updateLayout();
    fields->m_scrollLayer->setScrollY(currentScroll, false);

    fields->m_groupCountLabel = CCLabelBMFont::create(fmt::format("Remap Groups: {}", remapCounts.size()).c_str(), "chatFont.fnt");

    fields->m_groupCountLabel->setID("group-count-label"_spr);
    fields->m_groupCountLabel->setAnchorPoint({0, 1.f});
    fields->m_groupCountLabel->setColor({0, 0, 0});
    fields->m_groupCountLabel->setOpacity(200);
    fields->m_groupCountLabel->setScale(0.5f);
    fields->m_groupCountLabel->setPosition({fields->m_scrollLayer->getPositionX() - fields->m_scrollLayer->getContentWidth()/2, fields->m_scrollLayer->getPositionY() - fields->m_scrollLayer->getContentHeight()/2 - 2});

    m_mainLayer->addChild(fields->m_groupCountLabel);

    fields->m_scrollBar = AdvancedScrollBar::create(fields->m_scrollLayer, ScrollOrientation::VERTICAL);
    auto style = RoundedScrollStyle();
    style.m_track = [] () {
        auto track = RoundedScrollTrack::create();
        track->setClickColor({0, 0, 0, 0});
        track->setBackgroundColor({0, 0, 0, 0});
        return track;
    };

    fields->m_scrollBar->setStyle(style);
    fields->m_scrollBar->setContentSize({10, fields->m_scrollLayer->getContentHeight() - 10});
    fields->m_scrollBar->setPosition({fields->m_scrollLayer->getPositionX() + fields->m_scrollLayer->getContentWidth() / 2 - fields->m_scrollBar->getContentWidth() / 2 + 1, fields->m_scrollLayer->getPositionY()});
    fields->m_scrollBar->setZOrder(100);
    fields->m_scrollBar->setTouchPriority(fields->m_scrollLayer->getTouchPriority() - 10);

    m_mainLayer->addChild(fields->m_scrollBar);

    if (groupsMenu->getScaledContentHeight() <= 90) {
        fields->m_scrollLayer->setVerticalScroll(false);
        fields->m_scrollLayer->setHorizontalScrollWheel(false);
        fields->m_scrollBar->setVisible(false);
    }

    addObjectToPage(fields->m_scrollBar, 1);
    addObjectToPage(fields->m_groupCountLabel, 1);
    addObjectToPage(fields->m_scrollLayer, 1);
}

void IGVSetupRandAdvTriggerPopup::updateGroupIDButtons() {
    auto fields = m_fields.self();

    for (auto btn : CCArrayExt<CCNode, false>(m_groupButtons)) {
        btn->removeFromParent();
    }

    auto namedEditorGroupsMenu = m_buttonMenu->getChildByID("spaghettdev.named-editor-groups/groups-list-menu");
    if (namedEditorGroupsMenu) {
        namedEditorGroupsMenu->setVisible(false);
    }

    m_groupButtons->removeAllObjects();

    float currentScroll = 0;

    if (fields->m_scrollLayer) {
        currentScroll = fields->m_scrollLayer->getScrollPoint().y;
        fields->m_scrollLayer->removeFromParent();
    }

    if (fields->m_scrollBar) {
        fields->m_scrollBar->removeFromParent();
    }

    if (fields->m_groupCountLabel) {
        fields->m_groupCountLabel->removeFromParent();
    }

    std::vector<ChanceObject> chanceObjects;

    auto obj = static_cast<RandTriggerGameObject*>(m_gameObject);
    auto totalChance = obj->getTotalChance();

    for (auto chance : obj->m_chanceObjects) {
        chance.m_unk00c = totalChance;
        chanceObjects.push_back(chance);
    }
    
    std::sort(chanceObjects.begin(), chanceObjects.end(), [] (const ChanceObject& left, const ChanceObject& right) {
        return left.m_groupID < right.m_groupID;
    });

    auto groupsMenu = CCMenu::create();

    for (auto& obj : chanceObjects) {

        std::string sprite = "GJ_button_04.png";
        if (!m_gameObject && obj.m_chance < 1) {
            sprite = "GJ_button_05.png";
        }

        std::string label;

        if (obj.m_chance < 1) {
            label = fmt::format("{} - ?\n?%", obj.m_groupID);
        }
        else {
            int percent = static_cast<float>(obj.m_chance) / obj.m_unk00c * 100.f;
            label = fmt::format("{} - {}\n{}%", obj.m_groupID, obj.m_chance, percent);
        }

        auto size = CCSize{40.f, 30.f};
        bool hasName = false;
        if (tinker::utils::getMod<"spaghettdev.named-editor-groups">()) {
            size.height = 35.f;

            std::string name = NIDManager::getNameForID(NID::GROUP, obj.m_groupID).unwrapOrDefault();
            if (!name.empty()) {
                label += fmt::format("\n{}", name);
                hasName = true;
                size.width = 60.f;
            }
        }
        
        auto button = ButtonSprite::create(
            label.c_str(),
            size.width,
            0,
            0.35f,
            true,
            "bigFont.fnt",
            sprite.c_str(),
            size.height
        );

        if (obj.m_chance < 1) {
            button->m_label->setColor({255, 150, 0});
        }

        if (hasName) {
            if (button->m_label->getScale() >= (0.35f * 0.8f)) {
                button->m_label->setScale(button->m_label->getScale() * 0.8f);
            }
            button->m_label->setPositionX(9 + button->m_label->getScaledContentWidth() / 2);
        }

        auto menuItem = CCMenuItemSpriteExtra::create(
            button,
            this,
            menu_selector(SetupRandAdvTriggerPopup::onRemoveFromGroup)
        );

        menuItem->setTag(obj.m_groupID);

        groupsMenu->addChild(menuItem);
        m_groupButtons->addObject(menuItem);
    }

    auto layout = RowLayout::create();
    layout->setGap(10);
    layout->setAutoScale(false);
    layout->setGrowCrossAxis(true);
    layout->setCrossAxisOverflow(true);
    layout->setPadding({15, 10, 15, 10});

    if (ImprovedGroupView::getSetting<bool, "left-align">()) {
        layout->setAxisAlignment(AxisAlignment::Start);
    }

    groupsMenu->setLayout(layout);

    groupsMenu->setContentSize({350, 90});
    groupsMenu->setPosition({350/2.f, 0});
    groupsMenu->setAnchorPoint({0.5, 0});
    groupsMenu->updateLayout();
    groupsMenu->setID("groups-menu"_spr);

    if (groupsMenu->getScaledContentHeight() > 90) {
        layout->setPadding({15, 5, 15, 5});
        groupsMenu->updateLayout();
    }

    auto winSize = CCDirector::get()->getWinSize();

    fields->m_scrollLayer = AdvancedScrollLayer::create({350, 90});

    fields->m_scrollLayer->setContentSize({350, 90});
    fields->m_scrollLayer->setPosition({winSize.width/2, winSize.height/2 - 10});
    fields->m_scrollLayer->ignoreAnchorPointForPosition(false);
    fields->m_scrollLayer->addChild(groupsMenu);
    fields->m_scrollLayer->setID("groups-list-scroll-layer"_spr);
    fields->m_scrollLayer->setTouchPriority(-504);
    fields->m_scrollLayer->setScrollDelta(1.5f);
    fields->m_scrollLayer->getContentLayer()->setLayout(SimpleRowLayout::create()->setCrossAxisScaling(AxisScaling::Grow)->setCrossAxisAlignment(CrossAxisAlignment::Start));

    m_mainLayer->addChild(fields->m_scrollLayer);

    fields->m_scrollLayer->getContentLayer()->updateLayout();
    fields->m_scrollLayer->setScrollY(currentScroll, false);

    fields->m_scrollBar = AdvancedScrollBar::create(fields->m_scrollLayer, ScrollOrientation::VERTICAL);
    auto style = RoundedScrollStyle();
    style.m_track = [] () {
        auto track = RoundedScrollTrack::create();
        track->setClickColor({0, 0, 0, 0});
        track->setBackgroundColor({0, 0, 0, 0});
        return track;
    };

    fields->m_scrollBar->setStyle(style);
    fields->m_scrollBar->setContentSize({10, fields->m_scrollLayer->getContentHeight() - 10});
    fields->m_scrollBar->setPosition({fields->m_scrollLayer->getPositionX() + fields->m_scrollLayer->getContentWidth() / 2 - fields->m_scrollBar->getContentWidth() / 2 + 1, fields->m_scrollLayer->getPositionY()});
    fields->m_scrollBar->setZOrder(100);
    fields->m_scrollBar->setTouchPriority(fields->m_scrollLayer->getTouchPriority() - 10);

    m_mainLayer->addChild(fields->m_scrollBar);

    fields->m_groupCountLabel = CCLabelBMFont::create(fmt::format("Chance Groups: {}", chanceObjects.size()).c_str(), "chatFont.fnt");

    fields->m_groupCountLabel->setID("group-count-label"_spr);
    fields->m_groupCountLabel->setAnchorPoint({0, 1.f});
    fields->m_groupCountLabel->setColor({0, 0, 0});
    fields->m_groupCountLabel->setOpacity(200);
    fields->m_groupCountLabel->setScale(0.5f);
    fields->m_groupCountLabel->setPosition({fields->m_scrollLayer->getPositionX() - fields->m_scrollLayer->getContentWidth()/2, fields->m_scrollLayer->getPositionY() - fields->m_scrollLayer->getContentHeight()/2 - 2});

    m_mainLayer->addChild(fields->m_groupCountLabel);

    if (groupsMenu->getScaledContentHeight() <= 90) {
        fields->m_scrollLayer->setVerticalScroll(false);
        fields->m_scrollLayer->setHorizontalScrollWheel(false);
        fields->m_scrollBar->setVisible(false);
    }

    if (tinker::utils::getMod<"spaghettdev.named-editor-groups">()) {
        runAction(CallFuncExt::create([this] {
            for (auto btn : CCArrayExt<CCNode, false>(m_groupButtons)) {
                btn->setVisible(true);
            }
        }));
    }
}