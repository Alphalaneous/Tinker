#include "modules/ImprovedGroupView.hpp"
#include "Geode/cocos/actions/CCActionInterval.h"
#include "Geode/cocos/touch_dispatcher/CCTouchDispatcher.h"
#include "Geode/ui/Button.hpp"
#include "Geode/utils/cocos.hpp"
#include "alphalaneous.alphas-ui-pack/Utils.hpp"
#include "ccTypes.h"
#include <Geode/Geode.hpp>
#include <alphalaneous.alphas-ui-pack/API.hpp>

using namespace geode::prelude;

namespace tinker::ui {

GroupLayout* GroupLayout::create() {
    auto ret = new GroupLayout();
    ret->autorelease();
    return ret;
}

void GroupLayout::apply(cocos2d::CCNode* on) {
    struct LineInfo {
        float width;
        float height;
        std::vector<CCNode*> nodes;
    };

    std::vector<LineInfo> lineInfos;

    float maxWidth = m_size.width - m_padding.left - m_padding.right;
    float totalHeight = 0.f;
    float totalWidth = 0.f;

    auto info = LineInfo{};

    for (int i = 0; i < on->getChildrenCount(); i++) {
        bool last = i == on->getChildrenCount() - 1;
        auto child = on->getChildrenExt()[i];

        float childWidth = child->getScaledContentWidth();

        if ((info.width + childWidth) > maxWidth) {
            info.width -= m_horizontalGap;
            totalHeight += (info.height + m_verticalGap);
            totalWidth = std::max(info.width, totalWidth);

            lineInfos.push_back(std::move(info));

            info = LineInfo{};
        }

        info.width += (childWidth + m_horizontalGap);
        info.height = std::max(info.height, child->getScaledContentHeight());
        info.nodes.push_back(child);

        if (last) {
            info.width -= m_horizontalGap;
            totalHeight += (info.height + m_verticalGap);
            totalWidth = std::max(info.width, totalWidth);

            lineInfos.push_back(std::move(info));
        }
    }
    totalHeight += (m_padding.top + m_padding.bottom);
    totalHeight -= m_verticalGap;
    totalHeight = std::max(m_size.height, totalHeight);

    on->setContentSize({m_size.width + m_padding.left + m_padding.right, totalHeight});

    float y = totalHeight - m_padding.top;

    for (const auto& info : lineInfos) {
        float x = m_centerAligned ? (m_size.width / 2.f - info.width / 2.f): m_padding.left;
        for (auto node : info.nodes) {
            node->setPosition({x + node->getScaledContentWidth() / 2.f, y - info.height / 2.f});
            x += node->getScaledContentWidth() + m_horizontalGap;
        }
        y -= (info.height + m_verticalGap);
    }
}

cocos2d::CCSize GroupLayout::getSizeHint(cocos2d::CCNode* on) const {
    return {};
}

GroupLayout* GroupLayout::setSize(const cocos2d::CCSize& size) {
    m_size = size;
    return this;
}

GroupLayout* GroupLayout::setHorizontalGap(float gap) {
    m_horizontalGap = gap;
    return this;
}

GroupLayout* GroupLayout::setVerticalGap(float gap) {
    m_verticalGap = gap;
    return this;
}

GroupLayout* GroupLayout::setPadding(const geode::Padding& padding) {
    m_padding = padding;
    return this;
}

GroupLayout* GroupLayout::setCenterAligned(bool centerAligned) {
    m_centerAligned = centerAligned;
    return this;
}

CCSize GroupLayout::getSize() const {
    return m_size;
}

float GroupLayout::getHorizontalGap() const {
    return m_horizontalGap;
}

float GroupLayout::getVerticalGap() const {
    return m_verticalGap;
}

geode::Padding GroupLayout::getPadding() const {
    return m_padding;
}

bool GroupLayout::isCenterAligned() const {
    return m_centerAligned;
}

GroupView* GroupView::create(const cocos2d::CCSize& size, geode::Function<void()> generateCallback) {
    auto ret = new GroupView();
    if (ret->init(size, std::move(generateCallback))) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool GroupView::init(const cocos2d::CCSize& size, geode::Function<void()> generateCallback) {
    m_generateCallback = std::move(generateCallback);

    setContentSize(size);
    setAnchorPoint({0.5f, 0.5f});

    m_layout = tinker::ui::GroupLayout::create()
        ->setSize(size);

    m_scrollLayer = AdvancedScrollLayer::create(size);

    m_scrollLayer->setContentSize(size);
    m_scrollLayer->setPosition(size / 2.f);
    m_scrollLayer->setID("group-list"_spr);
    m_scrollLayer->setHorizontalScrollWheel(false);
    m_scrollLayer->getContentLayer()->setLayout(m_layout);

    addChild(m_scrollLayer);

    m_scrollBar = AdvancedScrollBar::create(m_scrollLayer, ScrollOrientation::Vertical);
    auto style = RoundedScrollStyle();
    style.m_track = [] {
        auto track = RoundedScrollTrack::create();
        track->setClickColor({0, 0, 0, 0});
        track->setBackgroundColor({0, 0, 0, 0});
        return track;
    };

    m_scrollBar->setID("group-scroll-bar"_spr);
    m_scrollBar->setStyle(style);
    m_scrollBar->setContentSize({10.f, m_scrollLayer->getContentHeight() - 10.f});
    m_scrollBar->setPosition({m_scrollLayer->boundingBox().getMaxX() - m_scrollBar->getContentWidth() / 2.f + 1.f, m_scrollLayer->getPositionY()});
    m_scrollBar->setZOrder(100);

    addChild(m_scrollBar);

    return true;
}

void GroupView::setBGScale(float bgScale) {
    m_bgScale = bgScale;
}

void GroupView::onEnter() {
    CCNode::onEnter();

    CCTouchDispatcher::get()->addTargetedDelegate(this, 0, true);
}

void GroupView::onExit() {
    CCNode::onExit();
    m_hoveredButton = nullptr;
    CCTouchDispatcher::get()->removeDelegate(this);
}

void GroupView::hoverDisplay(geode::Button* button, bool hovering) {
    if (!button) return;

    auto hovered = button->getUserFlag("hovered"_spr);
    if (hovered == hovering) return;

    auto display = button->getDisplayNode();
    display->stopAllActions();
    display->runAction(
        CCScaleTo::create(
            hovering ? 0.05f : 0.04f,
            (hovering ? 1.05f : 1.f) * m_bgScale
        )
    );

    button->setColor(hovering ? ccColor3B{220, 220, 220} : ccColor3B{255, 255, 255});
    button->setUserFlag("hovered"_spr, hovering);
}

void GroupView::checkButtons() {
    if (!ImprovedGroupView::getSetting<bool, "hover-animation">()) return;

    geode::Button* origButton = m_hoveredButton;
    m_hoveredButton = nullptr;
    
    for (auto btn : m_scrollLayer->getContentLayer()->getChildrenExt<geode::Button>()) {
        if (isPointInsideNode(btn, getMousePos())) {
            m_hoveredButton = btn;
            break;
        }
    }

    if (m_hoveredButton) {
        hoverDisplay(m_hoveredButton, true);
    }

    if (!m_hoveredButton || m_hoveredButton != origButton) {
        hoverDisplay(origButton, false);
    }
}

bool GroupView::hoverEntered(Hover* hover) {
    if (!nodeIsVisible(this) || !isPointInsideNode(this, hover->getLocation())) return false;

    checkButtons();

    return true;
}

void GroupView::hoverMoved(Hover* hover) {
    checkButtons();
}

void GroupView::hoverExited(Hover* hover) {
    checkButtons();
}

GroupLayout* GroupView::getGroupLayout() {
    return m_layout;
}

void GroupView::addButton(geode::Button* button) {
    m_scrollLayer->getContentLayer()->addChild(button);
}

void GroupView::generate() {
    m_scrollLayer->getContentLayer()->removeAllChildren();
    m_hoveredButton = nullptr;

    if (m_generateCallback) {
        m_generateCallback();
    }

    m_scrollLayer->getContentLayer()->updateLayout();

    bool isOnePage = m_scrollLayer->getContentLayer()->getContentHeight() <= m_scrollLayer->getContentHeight() + 5.f;

    m_scrollLayer->setVerticalScroll(!isOnePage);
    m_scrollLayer->setDraggingEnabled(!isOnePage);
    m_scrollBar->setVisible(!isOnePage);

    checkButtons();
}

}

bool ImprovedGroupView::onToggled(bool state) {
    return true;
}

bool ImprovedGroupView::onSettingChanged(std::string_view key, const matjson::Value& value) {
    return true;
}

void IGVIDNameBrowserPopup::onClose(cocos2d::CCObject* sender) {
    Ref self = this;
    IDNameBrowserPopup::onClose(sender);
    ClosedEvent().send();
}

bool IGVSetGroupIDLayer::init(GameObject* obj, cocos2d::CCArray* objs) {
    if (!SetGroupIDLayer::init(obj, objs)) return false;

    auto fields = m_fields.self();
    auto winSize = CCDirector::get()->getWinSize();

    if (auto node = m_mainLayer->getChildByID("group-list-scroll-layer")) {
        node->setVisible(false);
    }

    fields->m_groupView = tinker::ui::GroupView::create({360.f, 68.f}, [this, fields] {
        std::map<int, int> allGroups{};
        std::unordered_map<int, int> allParentGroups{};
        std::unordered_map<int, int> allTempParentGroups{};

        int count = 0;

        auto populate = [&] (const ImprovedGroupView::GroupData& data) {
            for (int group : data.parentGroups) {
                if (group == 0) continue;
                allParentGroups[group]++;
            }
            for (int group : data.tempParentGroups) {
                if (group == 0) continue;
                allTempParentGroups[group]++;
            }

            for (int group : data.groups) {
                if (group == 0) continue;
                allGroups[group]++;
            }

            count++;
        };

        if (!m_targetObjects || m_targetObjects->count() == 0) {
            if (m_targetObject) {
                populate(parseObjGroups(m_targetObject));
            }
        }
        else {
            for (auto obj : CCArrayExt<GameObject*>(m_targetObjects)) {
                populate(parseObjGroups(obj));
            }
        }

        auto mode = fields->m_modeButton->getValue();

        float scale = 1.f;
        float buttonWidth = 46.f;

        auto leftAlign = ImprovedGroupView::getSetting<bool, "left-align">();
        auto layout = fields->m_groupView->getGroupLayout();

        layout->setCenterAligned(!leftAlign);
        layout->setPadding({41.f, 9.5f, 41.f, 9.5f});
        layout->setHorizontalGap(12.f);
        layout->setVerticalGap(9.f);

        if (mode == ImprovedGroupView::GroupMode::Names) {
            layout->setHorizontalGap(7.f);
        }
        else if (allGroups.size() > 10) {
            layout->setPadding({12.5f, 11.5f, 12.5f, 11.5f});
            layout->setHorizontalGap(6.f);

            scale = 0.9f;
            buttonWidth = 31.f;
        }

        fields->m_groupView->setBGScale(scale);

        for (const auto& [k, v] : allGroups) {
            bool isParent = allParentGroups.count(k);
            bool isTempParent = allTempParentGroups.count(k);
            bool isAlwaysPresent = v == count;

            std::string texture = "GJ_button_04.png";

            if (!isAlwaysPresent) texture = "GJ_button_05.png";
            if (isTempParent) texture = "GJ_button_02.png";
            if (isParent) texture = "GJ_button_03.png";

            auto bg = NineSlice::create(texture);
            bg->setScale(scale);
            bg->setID("background-sprite"_spr);

            auto label = geode::Label::create(numToString(k), "goldFont.fnt");
            label->setScale(0.5f);
            label->setID("group-label"_spr);
            label->setAnchorPoint({0.5f, 0.5f});

            float width = buttonWidth;
            if (mode == ImprovedGroupView::GroupMode::Names) {
                width = std::max(buttonWidth, label->getScaledContentWidth() + 16.f);
            }

            bg->setContentSize({width, 20.f});
            label->setPosition({bg->getContentWidth() / 2.f, bg->getContentHeight() / 2.f + 1.5f});

            bg->addChild(label);

            if (mode == ImprovedGroupView::GroupMode::Names) {
                auto name = LevelEditorLayer::get()->m_levelSettings->getIDName(k, 1);
                GLubyte opacity = 255;
                float scale = 0.35f;
                if (name.empty()) {
                    name = "(Unnamed)";
                    opacity = 150;
                    scale = 0.2f;
                }
                label->setPosition({8.f, bg->getContentHeight() / 2.f + 1.5f});
                label->setAnchorPoint({0.f, 0.5f});
                label->validate();

                auto nameLabel = geode::Label::create(name, "bigFont.fnt");
                nameLabel->setLimitLabelWidth(150.f, scale, 0.05f);
                nameLabel->setAnchorPoint({0.f, 0.5f});
                nameLabel->setID("name-label"_spr);
                nameLabel->setOpacity(opacity);
                nameLabel->setPosition({label->boundingBox().getMaxX() + 4.f, bg->getContentHeight() / 2.f + 3.f * nameLabel->getScale()});
                nameLabel->validate();

                bg->addChild(nameLabel);
                bg->setContentWidth(nameLabel->boundingBox().getMaxX() + 8.f);
            }

            auto button = geode::Button::createWithNode(bg, [this] (auto sender) {
                onRemoveFromGroup(sender);
            });
            button->setID(fmt::format("group-{}-button"_spr, k));
            button->setTag(k);
            
            fields->m_groupView->addButton(button);
        }

        fields->m_groupCountLabel->setText(fmt::format("Groups: {}", allGroups.size()));
    });

    fields->m_groupView->setPosition({winSize.width / 2.f, winSize.height / 2.f - 16.8f});
    fields->m_groupView->setID("group-view"_spr);

    m_mainLayer->addChild(fields->m_groupView);

    fields->m_modeButton = tinker::ui::CycleButton<ImprovedGroupView, "id-layer/group-mode", ImprovedGroupView::GroupMode>::create(70.f, "Mode", {{ImprovedGroupView::GroupMode::IDs, "IDs"}, {ImprovedGroupView::GroupMode::Names, "Names"}}, [this, fields] (ImprovedGroupView::GroupMode mode) {
        fields->m_groupView->generate();
    });

    fields->m_modeButton->setScale(0.75f);
    fields->m_modeButton->setID("mode-button"_spr);

    m_mainLayer->addChild(fields->m_modeButton);

    fields->m_groupCountLabel = geode::Label::create("chatFont.fnt");
    fields->m_groupCountLabel->setID("group-count-label"_spr);
    fields->m_groupCountLabel->setAnchorPoint({0.f, 0.5f});
    fields->m_groupCountLabel->setColor({0, 0, 0});
    fields->m_groupCountLabel->setOpacity(200);
    fields->m_groupCountLabel->setScale(0.5f);
    m_mainLayer->addChild(fields->m_groupCountLabel);

    auto zLayerLabel = m_mainLayer->getChildByID("z-layer-label");
    auto groupsBG = m_mainLayer->getChildByID("groups-bg");

    if (zLayerLabel && groupsBG) {
        auto labelPos = zLayerLabel->getPosition();
        auto groupsBGSize = groupsBG->getContentSize();
        fields->m_groupCountLabel->setPosition({labelPos.x - groupsBGSize.width / 2.f, labelPos.y + 6.f});

        fields->m_modeButton->setPosition({labelPos.x + groupsBGSize.width / 2.f - fields->m_modeButton->getScaledContentWidth() / 2.f - 5.f, labelPos.y});
    }

    fields->m_groupView->generate();

    addEventListener(IGVIDNameBrowserPopup::ClosedEvent(), [this, fields] {
        fields->m_groupView->generate();
    });

    return true;
}

void IGVSetGroupIDLayer::updateGroupIDButtons() {
    SetGroupIDLayer::updateGroupIDButtons();
    auto fields = m_fields.self();
    if (!fields->m_groupView) return;
    fields->m_groupView->generate();
}

ImprovedGroupView::GroupData IGVSetGroupIDLayer::parseObjGroups(GameObject* obj) {
    auto editor = LevelEditorLayer::get();

    ImprovedGroupView::GroupData data{};

    if (obj->m_groups) {
        data.groups = std::vector<int>{obj->m_groups->begin(), obj->m_groups->end()};
    }

    for (const auto& [k, v] : CCDictionaryExt<int, CCArray*>(editor->m_parentGroupIDs)) {
        if (k != obj->m_uniqueID) continue;
        for (auto val : CCArrayExt<CCInteger*>(v)) {
            auto arr = static_cast<CCArray*>(editor->m_parentGroupsDict->objectForKey(val->getValue()));
            if (!arr || arr->count() == 0) continue;
            
            auto first = arr->asExt<GameObject>()[0];
            if (first != obj) {
                data.tempParentGroups.push_back(val->getValue());
            }
            else {
                data.parentGroups.push_back(val->getValue());
            }
        }
    }

    return data;
}

bool IGVSetupSpawnPopup::init(EffectGameObject* object, cocos2d::CCArray* objects) {
    if (!SetupSpawnPopup::init(object, objects)) return false;
    auto fields = m_fields.self();
    auto winSize = CCDirector::get()->getWinSize();

    fields->m_groupView = tinker::ui::GroupView::create({360.f, 90.f}, [this, fields] {
        m_remapGroups.clear();

        float currentScroll = 0.f;

        auto objects = m_gameObject ? CCArray::createWithObject(m_gameObject) : m_gameObjects;

        struct RemapEntry {
            int groupID = 0;
            int remapID = 0;

            bool operator<(const RemapEntry& rhs) const {
                return std::tie(groupID, remapID) < std::tie(rhs.groupID, rhs.remapID);
            }
        };

        std::map<RemapEntry, int> remapCounts{};

        for (auto obj : CCArrayExt<GameObject*>(objects)) {
            for (const auto& pair : static_cast<SpawnTriggerGameObject*>(obj)->m_remapObjects) {
                auto entry = RemapEntry{pair.m_groupID, pair.m_chance};
                remapCounts[entry]++;
            }
        }

        auto mode = fields->m_modeButton->getValue();
        auto type = static_cast<int>(fields->m_nameType);
        
        float buttonWidth = 40.f;

        auto layout = fields->m_groupView->getGroupLayout();
        auto leftAlign = ImprovedGroupView::getSetting<bool, "left-align">();

        layout->setCenterAligned(!leftAlign);
        layout->setPadding({20.f, 11.f, 20.f, 11.f});
        layout->setHorizontalGap(10.f);
        layout->setVerticalGap(8.f);

        if (mode == ImprovedGroupView::GroupMode::Names) {
            layout->setPadding({11.f, 11.f, 11.f, 11.f});
            layout->setHorizontalGap(5.f);

            buttonWidth = 15.f;
        }
        else if (remapCounts.size() > 10) {
            layout->setPadding({7.f, 11.f, 7.f, 11.f});
            layout->setHorizontalGap(4.f);

            buttonWidth = 15.f;
        }

        int index = 0;
        for (const auto& [entry, count] : remapCounts) {
            std::string sprite = "GJ_button_05.png";
            
            if (entry.groupID == m_remapOriginalID && entry.remapID == m_remapNewID) {
                sprite = "GJ_button_03.png";
            }
            else if (count == objects->count()) {
                sprite = "GJ_button_04.png";
            }
            
            auto bg = NineSlice::create(sprite);
            bg->setContentSize({buttonWidth + 16.f, 30.f});
            bg->setID("background-sprite"_spr);

            std::string font = mode == ImprovedGroupView::GroupMode::Names ? "goldFont.fnt" : "bigFont.fnt";

            auto origIDLabel = geode::Label::create(numToString(entry.groupID), font);
            origIDLabel->setID("original-group-id-label"_spr);

            auto newIDLabel = geode::Label::create(numToString(entry.remapID), font);
            newIDLabel->setID("remap-group-id-label"_spr);

            float topY = bg->getContentHeight() / 2.f + bg->getContentHeight() / 4.f - 1.75f;
            float bottomY = bg->getContentHeight() / 2.f - bg->getContentHeight() / 4.f + 1.75f;

            float x = bg->getContentWidth() / 2.f;

            if (mode == ImprovedGroupView::GroupMode::Names) {
                origIDLabel->setScale(0.4f);
                newIDLabel->setScale(0.4f);

                origIDLabel->setAnchorPoint({1.f, 0.5f});
                newIDLabel->setAnchorPoint({1.f, 0.5f});

                x = std::max(origIDLabel->getScaledContentWidth(), newIDLabel->getScaledContentWidth()) + 8.f;

                auto name1 = LevelEditorLayer::get()->m_levelSettings->getIDName(entry.groupID, type);
                GLubyte opacity1 = 255;
                float scale1 = 0.25f;
                if (name1.empty()) {
                    name1 = "(Unnamed)";
                    opacity1 = 150;
                    scale1 = 0.2f;
                }

                auto origNameLabel = geode::Label::create(name1, "bigFont.fnt");
                origNameLabel->setLimitLabelWidth(150.f, scale1, 0.05f);
                origNameLabel->setAnchorPoint({0.f, 0.5f});
                origNameLabel->setID("original-name-label"_spr);
                origNameLabel->setOpacity(opacity1);
                origNameLabel->setPosition({x + 4.f, topY + 3.f * origNameLabel->getScale()});
                origNameLabel->validate();

                bg->addChild(origNameLabel);

                auto name2 = LevelEditorLayer::get()->m_levelSettings->getIDName(entry.remapID, type);
                GLubyte opacity2 = 255;
                float scale2 = 0.25f;
                if (name2.empty()) {
                    name2 = "(Unnamed)";
                    opacity2 = 150;
                    scale2 = 0.2f;
                }

                auto remapNameLabel = geode::Label::create(name2, "bigFont.fnt");
                remapNameLabel->setLimitLabelWidth(150.f, scale2, 0.05f);
                remapNameLabel->setAnchorPoint({0.f, 0.5f});
                remapNameLabel->setID("remap-name-label"_spr);
                remapNameLabel->setOpacity(opacity2);
                remapNameLabel->setPosition({x + 4.f, bottomY + 3.f * remapNameLabel->getScale()});
                remapNameLabel->validate();

                bg->addChild(remapNameLabel);

                float max = std::max(origNameLabel->getScaledContentWidth(), remapNameLabel->getScaledContentWidth());

                bg->setContentSize({x + 4.f + max + 8.f, 30.f});
            }
            else {
                origIDLabel->setLimitLabelWidth(buttonWidth, 0.35f, 0.05f);
                newIDLabel->setLimitLabelWidth(buttonWidth, 0.35f, 0.05f);
            }

            origIDLabel->setPosition({x, topY + 3.f * origIDLabel->getScale()});
            newIDLabel->setPosition({x, bottomY + 3.f * newIDLabel->getScale()});

            bg->addChild(origIDLabel);
            bg->addChild(newIDLabel);

            auto button = geode::Button::createWithNode(bg, [this] (auto sender) {
                onSelectRemap(sender);
            });

            button->setTag(index);

            fields->m_groupView->addButton(button);

            m_remapGroups.emplace_back(entry.groupID);
            m_remapGroups.emplace_back(entry.remapID);

            index++;
        }

        fields->m_groupCountLabel->setText(fmt::format("Remap Groups: {}", remapCounts.size()));
    });

    fields->m_groupView->setID("group-view"_spr);
    fields->m_groupView->setVisible(false);
    fields->m_groupView->setPosition({winSize.width / 2.f, winSize.height / 2.f});
    m_mainLayer->addChild(fields->m_groupView);

    addObjectToPage(fields->m_groupView, 1);

    fields->m_modeButton = tinker::ui::CycleButton<ImprovedGroupView, "spawn/group-mode", ImprovedGroupView::GroupMode>::create(70.f, "Mode", {{ImprovedGroupView::GroupMode::IDs, "IDs"}, {ImprovedGroupView::GroupMode::Names, "Names"}}, [this, fields] (ImprovedGroupView::GroupMode mode) {
        fields->m_typeDropdown->setVisible(mode == ImprovedGroupView::GroupMode::Names);
        if (mode == ImprovedGroupView::GroupMode::Names) {
            addObjectToPage(fields->m_typeDropdown, 1);
        }
        else {
            removeObjectFromPage(fields->m_typeDropdown, 1);
        }

        fields->m_groupView->generate();
    });

    fields->m_modeButton->setScale(0.75f);
    fields->m_modeButton->setVisible(false);
    fields->m_modeButton->setID("mode-button"_spr);
    fields->m_modeButton->setPosition({fields->m_groupView->getPositionX() + fields->m_groupView->getContentWidth() / 2.f - 5.f - fields->m_modeButton->getScaledContentWidth() / 2.f, fields->m_groupView->getPositionY() - fields->m_groupView->getContentHeight() / 2.f - 3.f - fields->m_modeButton->getScaledContentHeight() / 2.f});

    addObjectToPage(fields->m_modeButton, 1);
    m_mainLayer->addChild(fields->m_modeButton);

    auto nameType = ImprovedGroupView::getSaved<Save::Global, int, "spawn/name-type">(1);

    fields->m_nameType = static_cast<ImprovedGroupView::NameType>(nameType);

    fields->m_typeDropdown = tinker::ui::Dropdown::create({"Group", "Item", "Block", "Timer", "Effect", "Control", "Gradient", "Enter"}, nameType - 1, [this, fields] (int index) {
        ImprovedGroupView::setSaved<Save::Global, "spawn/name-type">(index + 1);

        fields->m_nameType = static_cast<ImprovedGroupView::NameType>(index + 1);
        fields->m_groupView->generate();
    });
    fields->m_typeDropdown->setScale(0.75f);
    fields->m_typeDropdown->setVisible(false);
    fields->m_typeDropdown->setContentSize({70.f, 22.f});
    fields->m_typeDropdown->setPosition({fields->m_modeButton->boundingBox().getMinX() - 5.f - fields->m_typeDropdown->getScaledContentWidth() / 2.f, fields->m_groupView->getPositionY() - fields->m_groupView->getContentHeight() / 2.f - 4.5f - fields->m_typeDropdown->getScaledContentHeight() / 2.f});

    if (fields->m_modeButton->getValue() == ImprovedGroupView::GroupMode::Names) {
        addObjectToPage(fields->m_typeDropdown, 1);
    }
    m_mainLayer->addChild(fields->m_typeDropdown);

    fields->m_groupCountLabel = geode::Label::create(fmt::format("Remap Groups: 0"), "chatFont.fnt");

    fields->m_groupCountLabel->setID("group-count-label"_spr);
    fields->m_groupCountLabel->setAnchorPoint({0.f, 1.f});
    fields->m_groupCountLabel->setColor({0, 0, 0});
    fields->m_groupCountLabel->setOpacity(200);
    fields->m_groupCountLabel->setScale(0.5f);
    fields->m_groupCountLabel->setVisible(false);
    fields->m_groupCountLabel->setPosition({fields->m_groupView->getPositionX() - fields->m_groupView->getContentWidth() / 2.f, fields->m_groupView->getPositionY() - fields->m_groupView->getContentHeight() / 2.f - 2.f});

    addObjectToPage(fields->m_groupCountLabel, 1);
    m_mainLayer->addChild(fields->m_groupCountLabel);

    fields->m_groupView->generate();

    addEventListener(IGVIDNameBrowserPopup::ClosedEvent(), [this, fields] {
        fields->m_groupView->generate();
    });

    return true;
}

void IGVSetupSpawnPopup::updateRemapButtons(float dt) {
    // Since hooking queueUpdateButtons crashes on iOS, I have to run the original of this to maintain scheduling, then clean up
    SetupSpawnPopup::updateRemapButtons(dt);

    for (auto node : m_remapButtons->asExt<CCNode>()) {
        node->removeFromParent();
        getPageContainer(1)->removeObject(node, true);
    }
    m_remapButtons->removeAllObjects();

    auto fields = m_fields.self();
    if (!fields->m_groupView) return;
    fields->m_groupView->generate();
}

bool IGVSetupRandAdvTriggerPopup::init(RandTriggerGameObject* object, cocos2d::CCArray* objects) {
    if (!SetupRandAdvTriggerPopup::init(object, objects)) return false;

    auto fields = m_fields.self();
    auto winSize = CCDirector::get()->getWinSize();

    fields->m_groupView = tinker::ui::GroupView::create({350.f, 90.f}, [this, fields] {
        std::vector<ChanceObject> chanceObjects{};

        auto obj = static_cast<RandTriggerGameObject*>(m_gameObject);
        auto totalChance = obj->getTotalChance();

        for (auto chance : obj->m_chanceObjects) {
            chance.m_unk00c = totalChance;
            chanceObjects.push_back(chance);
        }
        
        std::sort(chanceObjects.begin(), chanceObjects.end(), [] (const ChanceObject& left, const ChanceObject& right) {
            return left.m_groupID < right.m_groupID;
        });

        auto mode = fields->m_modeButton->getValue();

        auto layout = fields->m_groupView->getGroupLayout();
        auto leftAlign = ImprovedGroupView::getSetting<bool, "left-align">();

        layout->setCenterAligned(!leftAlign);
        layout->setPadding({15.f, 11.f, 15.f, 11.f});
        layout->setHorizontalGap(10.f);
        layout->setVerticalGap(8.f);

        if (mode == ImprovedGroupView::GroupMode::Names) {
            layout->setHorizontalGap(4.f);
            layout->setPadding({8.f, 11.f, 8.f, 11.f});
        }

        for (auto& obj : chanceObjects) {
            std::string sprite = "GJ_button_04.png";
            if (!m_gameObject && obj.m_chance < 1) {
                sprite = "GJ_button_05.png";
            }

            auto bg = NineSlice::create(sprite);
            bg->setContentSize({56.f, 30.f});
            bg->setID("background-sprite"_spr);

            if (mode == ImprovedGroupView::GroupMode::Names) {
                float topY = bg->getContentHeight() / 2.f + bg->getContentHeight() / 4.f - 1.75f;
                float bottomY = bg->getContentHeight() / 2.f - bg->getContentHeight() / 4.f + 1.75f;

                auto groupLabel = geode::Label::create(numToString(obj.m_groupID), "goldFont.fnt");
                groupLabel->setScale(0.4f);
                groupLabel->setID("group-label"_spr);
                groupLabel->setAnchorPoint({0.f, 0.5f});
                groupLabel->setPosition({8.f, bg->getContentHeight() / 2.f + 3.f * groupLabel->getScale()});
                groupLabel->validate();

                bg->addChild(groupLabel);
                                
                std::string chanceText = "";
                if (obj.m_chance < 1) {
                    chanceText = "?% (?)";
                }
                else {
                    int percent = static_cast<float>(obj.m_chance) / obj.m_unk00c * 100.f;
                    chanceText = fmt::format("{}% ({})", percent, obj.m_chance);
                }

                auto name = LevelEditorLayer::get()->m_levelSettings->getIDName(obj.m_groupID, 1);
                GLubyte opacity = 255;
                float scale = 0.3f;
                if (name.empty()) {
                    name = "(Unnamed)";
                    opacity = 150;
                    scale = 0.25f;
                }

                auto nameLabel = geode::Label::create(name, "bigFont.fnt");
                nameLabel->setLimitLabelWidth(150.f, scale, 0.05f);
                nameLabel->setAnchorPoint({0.f, 0.5f});
                nameLabel->setID("name-label"_spr);
                nameLabel->setOpacity(opacity);
                nameLabel->setPosition({groupLabel->boundingBox().getMaxX() + 4.f, topY + 3.f * nameLabel->getScale()});
                nameLabel->validate();

                bg->addChild(nameLabel);

                bg->setContentWidth(nameLabel->boundingBox().getMaxX() + 8.f);

                auto chanceLabel = geode::Label::create(chanceText, "bigFont.fnt");
                chanceLabel->setScale(0.25f);
                chanceLabel->setID("chance-label"_spr);
                chanceLabel->setAnchorPoint({0.f, 0.5f});
                chanceLabel->setPosition({groupLabel->boundingBox().getMaxX() + 4.f, bottomY + 3.f * chanceLabel->getScale()});
                chanceLabel->setLimitLabelWidth(150.f, 0.25f, 0.05f);

                bg->addChild(chanceLabel);

                if (obj.m_chance < 1) {
                    chanceLabel->setColor({255, 150, 0});
                }
            }
            else {
                std::string text = "";

                if (obj.m_chance < 1) {
                    text = fmt::format("{} - ?\n?%", obj.m_groupID);
                }
                else {
                    int percent = static_cast<float>(obj.m_chance) / obj.m_unk00c * 100.f;
                    text = fmt::format("{} - {}\n{}%", obj.m_groupID, obj.m_chance, percent);
                }

                auto label = geode::Label::create(text, "bigFont.fnt");
                label->setScale(0.35f);
                label->setID("group-label"_spr);
                label->setAnchorPoint({0.5f, 0.5f});
                label->setPosition({bg->getContentWidth() / 2.f, bg->getContentHeight() / 2.f + 1.5f});

                bg->addChild(label);
                
                label->setLimitLabelWidth(bg->getContentWidth() - 16.f, 0.35f, 0.05f);\
                if (obj.m_chance < 1) {
                    label->setColor({255, 150, 0});
                }
            }

            auto button = geode::Button::createWithNode(bg, [this] (auto sender) {
                onRemoveFromGroup(sender);
            });

            button->setTag(obj.m_groupID);
            fields->m_groupView->addButton(button);
        }

        fields->m_groupCountLabel->setText(fmt::format("Chance Groups: {}", chanceObjects.size()));
    });

    fields->m_groupView->setID("group-view"_spr);
    fields->m_groupView->setPosition({winSize.width / 2.f, winSize.height / 2.f - 10.f});
    m_mainLayer->addChild(fields->m_groupView);

    fields->m_modeButton = tinker::ui::CycleButton<ImprovedGroupView, "advanced-random/group-mode", ImprovedGroupView::GroupMode>::create(70.f, "Mode", {{ImprovedGroupView::GroupMode::IDs, "IDs"}, {ImprovedGroupView::GroupMode::Names, "Names"}}, [this, fields] (ImprovedGroupView::GroupMode mode) {
        fields->m_groupView->generate();
    });

    fields->m_modeButton->setScale(0.75f);
    fields->m_modeButton->setID("mode-button"_spr);
    fields->m_modeButton->setPosition({fields->m_groupView->getPositionX() + fields->m_groupView->getContentWidth() / 2.f - 5.f - fields->m_modeButton->getScaledContentWidth() / 2.f, fields->m_groupView->getPositionY() - fields->m_groupView->getContentHeight() / 2.f - 3.f - fields->m_modeButton->getScaledContentHeight() / 2.f});

    m_mainLayer->addChild(fields->m_modeButton);

    fields->m_groupCountLabel = geode::Label::create(fmt::format("Chance Groups: 0"), "chatFont.fnt");

    fields->m_groupCountLabel->setID("group-count-label"_spr);
    fields->m_groupCountLabel->setAnchorPoint({0.f, 1.f});
    fields->m_groupCountLabel->setColor({0, 0, 0});
    fields->m_groupCountLabel->setOpacity(200);
    fields->m_groupCountLabel->setScale(0.5f);
    fields->m_groupCountLabel->setPosition({fields->m_groupView->getPositionX() - fields->m_groupView->getContentWidth() / 2.f, fields->m_groupView->getPositionY() - fields->m_groupView->getContentHeight() / 2.f - 2.f});

    m_mainLayer->addChild(fields->m_groupCountLabel);

    fields->m_groupView->generate();

    addEventListener(IGVIDNameBrowserPopup::ClosedEvent(), [this, fields] {
        fields->m_groupView->generate();
    });

    return true;
}

void IGVSetupRandAdvTriggerPopup::updateGroupIDButtons() {
    auto fields = m_fields.self();
    if (!fields->m_groupView) return;
    fields->m_groupView->generate();
}

bool IGVSetupSequenceTriggerPopup::init(SequenceTriggerGameObject* object) {
    if (!SetupSequenceTriggerPopup::init(object)) return false;

    auto fields = m_fields.self();
    auto winSize = CCDirector::get()->getWinSize();

    fields->m_groupView = tinker::ui::GroupView::create({370.f, 76.f}, [this, fields] {
        int selectedIndex = -1;
        if (m_selectedButton) {
            selectedIndex = m_selectedButton->getTag();
            fields->m_selectedButton = nullptr;
            m_selectedButton = nullptr;
        }

        auto object = static_cast<SequenceTriggerGameObject*>(m_gameObject);

        auto mode = fields->m_modeButton->getValue();

        auto layout = fields->m_groupView->getGroupLayout();
        layout->setPadding({30.f, 6.5f, 30.f, 6.5f});
        layout->setHorizontalGap(10.f);
        float width = 40.f;

        if (mode == ImprovedGroupView::GroupMode::Names) {
            layout->setPadding({6.5f, 6.5f, 6.5f, 6.5f});
            layout->setHorizontalGap(4.f);
            width = 15.f;
        }
        else if (object->m_chanceObjects.size() > 10) {
            layout->setPadding({18.f, 6.5f, 18.f, 6.5f});
            layout->setHorizontalGap(4.f);
            width = 15.f;
        }

        auto leftAlign = ImprovedGroupView::getSetting<bool, "left-align">();

        layout->setCenterAligned(!leftAlign);
        layout->setVerticalGap(6.f);

        auto winSize = CCDirector::get()->getWinSize();

        int index = 0;
        for (const auto& obj : object->m_chanceObjects) {
            std::string sprite = (index == selectedIndex) ? "GJ_button_03.png" : "GJ_button_04.png";

            auto bg = geode::NineSlice::create(sprite);
            bg->setContentSize({width + 16.f, 30.f});
            bg->setID("background-sprite"_spr);
            bg->setScale(0.95f);
            fields->m_groupView->setBGScale(0.95f);

            if (mode == ImprovedGroupView::GroupMode::Names) {
                float topY = bg->getContentHeight() / 2.f + bg->getContentHeight() / 4.f - 1.75f;
                float bottomY = bg->getContentHeight() / 2.f - bg->getContentHeight() / 4.f + 1.75f;

                auto groupLabel = geode::Label::create(numToString(obj.m_groupID), "goldFont.fnt");
                groupLabel->setScale(0.4f);
                groupLabel->setID("group-label"_spr);
                groupLabel->setAnchorPoint({0.f, 0.5f});
                groupLabel->setPosition({8.f, bg->getContentHeight() / 2.f + 3.f * groupLabel->getScale()});
                groupLabel->validate();

                bg->addChild(groupLabel);

                auto name = LevelEditorLayer::get()->m_levelSettings->getIDName(obj.m_groupID, 1);
                GLubyte opacity = 255;
                float scale = 0.3f;
                if (name.empty()) {
                    name = "(Unnamed)";
                    opacity = 150;
                    scale = 0.25f;
                }

                auto nameLabel = geode::Label::create(name, "bigFont.fnt");
                nameLabel->setLimitLabelWidth(150.f, scale, 0.05f);
                nameLabel->setAnchorPoint({0.f, 0.5f});
                nameLabel->setID("name-label"_spr);
                nameLabel->setOpacity(opacity);
                nameLabel->setPosition({groupLabel->boundingBox().getMaxX() + 4.f, topY + 3.f * nameLabel->getScale()});
                nameLabel->validate();

                bg->addChild(nameLabel);

                bg->setContentWidth(nameLabel->boundingBox().getMaxX() + 8.f);

                auto countLabel = geode::Label::create(fmt::format("{}x", obj.m_chance), "bigFont.fnt");
                countLabel->setScale(0.25f);
                countLabel->setID("count-label"_spr);
                countLabel->setAnchorPoint({0.f, 0.5f});
                countLabel->setPosition({groupLabel->boundingBox().getMaxX() + 4.f, bottomY + 3.f * countLabel->getScale()});
                countLabel->setLimitLabelWidth(150.f, 0.25f, 0.05f);

                bg->addChild(countLabel);
            }
            else {
                std::string text = fmt::format("{}\n{}", obj.m_groupID, obj.m_chance);

                auto label = geode::Label::create(text, "bigFont.fnt");
                label->setLimitLabelWidth(width, 0.35f, 0.05f);
                label->setPosition({bg->getContentWidth() / 2.f, bg->getContentHeight() / 2.f + 3.f * label->getScale()});
                label->setID("group-label"_spr);

                bg->addChild(label);
            }

            auto button = geode::Button::createWithNode(bg, [this, fields] (auto sender) {
                auto updateNineslice = [] (geode::Button* button, ZStringView file) {
                    auto nineSlice = static_cast<geode::NineSlice*>(button->getDisplayNode());
                    
                    Ref children = nineSlice->getChildren()->shallowCopy();
                    Ref ref = nineSlice;

                    nineSlice->removeFromParent();

                    auto newNineSlice = geode::NineSlice::create(file);
                    newNineSlice->setContentSize(nineSlice->getContentSize());
                    newNineSlice->setScale(nineSlice->getScale());
                    newNineSlice->setID(nineSlice->getID());
                    newNineSlice->setPosition(nineSlice->getPosition());

                    for (auto child : children->asExt<CCNode>()) {
                        newNineSlice->addChild(child);
                    }

                    button->setDisplayNode(newNineSlice);
                };
                
                if (fields->m_selectedButton) {
                    updateNineslice(fields->m_selectedButton, "GJ_button_04.png");
                }
                if (fields->m_selectedButton == sender) {
                    fields->m_selectedButton = nullptr;
                    m_selectedButton = nullptr;
                    return;
                }
                fields->m_selectedButton = sender;
                m_selectedButton = fields->m_fakeButton;
                m_selectedButton->setTag(fields->m_selectedButton->getTag());

                updateNineslice(fields->m_selectedButton, "GJ_button_03.png");
            });

            button->setTag(index);
            fields->m_groupView->addButton(button);

            if (index == selectedIndex) {
                fields->m_selectedButton = button;
                m_selectedButton = fields->m_fakeButton;
                m_selectedButton->setTag(button->getTag());
            }
            index++;
        }

        fields->m_groupCountLabel->setText(fmt::format("Sequence Groups: {}", object->m_chanceObjects.size()));
    });

    CCScale9Sprite* groupBG = nullptr;

    for (auto child : m_mainLayer->getChildrenExt()) {
        auto scale9 = typeinfo_cast<CCScale9Sprite*>(child);
        if (!scale9) continue;

        if (child->getContentSize() == CCSize{370.f, 90.f}) {
            groupBG = scale9;
            break;
        }
    }

    if (groupBG) {
        groupBG->setContentHeight(76.f);
        groupBG->setPositionY(winSize.height / 2.f + 22.f);
    }

    fields->m_groupView->setID("group-view"_spr);
    fields->m_groupView->setPosition({winSize.width / 2.f, winSize.height / 2.f + 22.f});
    m_mainLayer->addChild(fields->m_groupView);

    fields->m_modeButton = tinker::ui::CycleButton<ImprovedGroupView, "sequence/group-mode", ImprovedGroupView::GroupMode>::create(70.f, "Mode", {{ImprovedGroupView::GroupMode::IDs, "IDs"}, {ImprovedGroupView::GroupMode::Names, "Names"}}, [this, fields] (ImprovedGroupView::GroupMode mode) {
        fields->m_groupView->generate();
    });

    fields->m_modeButton->setScale(0.75f);
    fields->m_modeButton->setID("mode-button"_spr);
    fields->m_modeButton->setPosition({fields->m_groupView->getPositionX() + fields->m_groupView->getContentWidth() / 2.f - 5.f - fields->m_modeButton->getScaledContentWidth() / 2.f, fields->m_groupView->getPositionY() - fields->m_groupView->getContentHeight() / 2.f - 3.f - fields->m_modeButton->getScaledContentHeight() / 2.f});

    m_mainLayer->addChild(fields->m_modeButton);

    fields->m_groupCountLabel = geode::Label::create(fmt::format("Chance Groups: 0"), "chatFont.fnt");

    fields->m_groupCountLabel->setID("group-count-label"_spr);
    fields->m_groupCountLabel->setAnchorPoint({0.f, 1.f});
    fields->m_groupCountLabel->setColor({0, 0, 0});
    fields->m_groupCountLabel->setOpacity(200);
    fields->m_groupCountLabel->setScale(0.5f);
    fields->m_groupCountLabel->setPosition({fields->m_groupView->getPositionX() - fields->m_groupView->getContentWidth() / 2.f, fields->m_groupView->getPositionY() - fields->m_groupView->getContentHeight() / 2.f - 2.f});

    m_mainLayer->addChild(fields->m_groupCountLabel);

    fields->m_fakeButton = CCMenuItemSpriteExtra::create(CCNode::create(), nullptr, nullptr);

    fields->m_groupView->generate();

    addEventListener(IGVIDNameBrowserPopup::ClosedEvent(), [this, fields] {
        fields->m_groupView->generate();
    });

    return true;
}

void IGVSetupSequenceTriggerPopup::onSelect(cocos2d::CCObject* sender) {
    // do nothing cuz I use geode::Button
}

void IGVSetupSequenceTriggerPopup::onDeleteSelected(cocos2d::CCObject* sender) {
    SetupSequenceTriggerPopup::onDeleteSelected(sender);
    m_fields->m_selectedButton = nullptr;
}

void IGVSetupSequenceTriggerPopup::updateGroupIDButtons() {
    auto fields = m_fields.self();
    if (!fields->m_groupView) return;
    fields->m_groupView->generate();
}