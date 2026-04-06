#include "ImprovedGroupView.hpp"
#include "../../include/ImprovedGroupView.hpp"
#include <spaghettdev.named-editor-groups/api/NIDManager.hpp>

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

    if (fields->m_scrollLayer) fields->m_scrollLayer->removeFromParent();

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
    fields->m_scrollLayer->setTouchPriority(-130);

    m_mainLayer->addChild(fields->m_scrollLayer);

    if (menuContainer->getScaledContentHeight() <= 67) {
        fields->m_scrollLayer->setVerticalScroll(false);
        fields->m_scrollLayer->setHorizontalScrollWheel(false);
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
