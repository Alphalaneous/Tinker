#include "modules/Gizmos/ColorChannelGizmo.hpp"
#include "modules/Gizmos/Gizmos.hpp"
#include "modules/ImprovedColorPicker.hpp"
#include "modules/RepeatingEditorButtons.hpp"
#include "nodes/ColorVisualButton.hpp"
#include "nodes/LabelledToggle.hpp"
#include "nodes/LazyColorRow.hpp"
#include "utils/Constants.hpp"

CCNode* ColorChannelGizmo::setup() {
    m_background = geode::NineSlice::create("simple-popup-square.png"_spr);
    m_background->setOpacity(160);
    m_background->setContentSize({10.f, 10.f});
    m_background->setLayout(SimpleRowLayout::create()
        ->setPadding({10.f, 10.f, 10.f, 10.f})
        ->setMainAxisScaling(AxisScaling::Fit)
        ->setCrossAxisScaling(AxisScaling::Fit)
        ->setGap(5.f)
    );
    m_background->setID("background"_spr);

    m_object->setCanEditObject(true);

    updateColorButtons();

    m_editPopupSize = CCSize{360.f, 240.f};
    m_editPopupTitle = "Setup Color Channel Gizmo";

    return m_background;
}

void ColorChannelGizmo::onEditObject() {
    tinker::ui::EditGizmoPopup<ColorChannelGizmo>::create(this)->show();
}

void ColorChannelGizmo::loadData(std::optional<const matjson::Value> data) {
    if (!data) {
        m_colorChannels.insert(1);
    }
    else {
        auto arrRes = data.value()["channels"].asArray();
        if (arrRes) {
            auto arr = arrRes.unwrap();
            for (auto val : arr) {
                auto numRes = val.asInt();
                if (numRes) {
                    m_colorChannels.insert(numRes.unwrap());
                }
            }
        }
        auto rowsRes = data.value()["rows"].asInt();
        if (rowsRes) {
            auto rows = rowsRes.unwrap();
            if (rows > 0) {
                m_rows = rows;
            }
        }
        m_isHorizontal = data.value()["is-horizontal"].asBool().unwrapOrDefault();
    }
}

// there seems to be a rare crash when updating the buttons, hard to reproduce, will need testing
void ColorChannelGizmo::updateColorButtons() {
    for (auto menu : m_colorsMenus) {
        menu->removeFromParent();
    }
    m_colorsMenus.clear();
    m_colorButtons.clear();

    if (m_colorChannels.empty()) {
        m_background->setContentSize({48.4f, 48.4f});
        m_object->updateBounds();
        return;
    }

    auto layout = static_cast<SimpleAxisLayout*>(m_background->getLayout());
    layout->setAxis(m_isHorizontal ? Axis::Column : Axis::Row);
    layout->setMainAxisDirection(m_isHorizontal ? AxisDirection::TopToBottom : AxisDirection::LeftToRight);

    int i = 0;
    int menuIdx = 0;
    CCMenu* currentMenu;
    for (auto channel : m_colorChannels) {
        if (i % m_rows == 0) {
            currentMenu = CCMenu::create();
            currentMenu->setID(fmt::format("color-menu-{}", menuIdx));
            currentMenu->setZOrder(menuIdx);
            currentMenu->setLayout(SimpleAxisLayout::create(m_isHorizontal ? Axis::Row : Axis::Column)
                ->setGap(5.f)
                ->setMainAxisScaling(AxisScaling::Fit)
                ->setCrossAxisScaling(AxisScaling::Fit)
            );

            m_colorsMenus.push_back(currentMenu);
            m_background->addChild(currentMenu);
            menuIdx++;
        }
        auto btn = tinker::ui::ColorVisualButton::create(EditorUI::get());
        btn->setColorData(channel);
        btn->setID(fmt::format("color-{}-button"_spr, channel));

        m_colorButtons.push_back(btn);
        currentMenu->addChild(btn);
        i++;
        if (i == 100) break;
    }

    for (const auto& menu : m_colorsMenus) {
        menu->updateLayout();
    }

    m_background->updateLayout();
    m_object->updateBounds();
}

matjson::Value ColorChannelGizmo::saveData() {
    matjson::Value data;
    data["channels"] = m_colorChannels;
    data["rows"] = m_rows;
    data["is-horizontal"] = m_isHorizontal;

    return data;
}

void ColorChannelGizmo::onRotate(float degrees) {
    for (const auto& btn : m_colorButtons) {
        btn->setRotation(-degrees);
    }
}

void ColorChannelGizmo::update(float dt) {
    for (const auto& btn : m_colorButtons) {
        btn->updateColor();
        btn->setRotation(-m_object->getRotation());
    }
}

void ColorChannelGizmo::setupEditPopup(geode::Popup* popup) {
    auto size = popup->m_mainLayer->getContentSize();
    popup->setOpacity(200);

    bool compactUI = ImprovedColorPicker::getSetting<bool, "compact-ui">();

    auto previewMenu = CCMenu::create();
    previewMenu->setContentSize({35.f, 40.f});
    previewMenu->ignoreAnchorPointForPosition(false);
    previewMenu->setAnchorPoint({0.5f, 0.5f});
    previewMenu->setPosition({size.width / 2.f + 210.f, size.height / 2.f - 90.f});
    previewMenu->setID("preview-menu"_spr);
    popup->m_mainLayer->addChild(previewMenu);

    auto liveLabel = geode::Label::create("Preview", "goldFont.fnt");
    liveLabel->setScale(0.3f);
    liveLabel->setAnchorPoint({0.5f, 0.f});
    liveLabel->setPosition({previewMenu->getContentWidth() / 2.f, 32.f});
    liveLabel->setID("preview-label"_spr);

    previewMenu->addChild(liveLabel);

    auto liveToggle = CCMenuItemExt::createTogglerWithStandardSprites(0.7f, [this] (CCMenuItemToggler* toggler) {
        for (const auto& spr : m_popupData.m_colorChannelSprites) {
            spr->setLive(!toggler->isToggled());
            spr->updateSprite();
        }
    });

    liveToggle->setPosition({previewMenu->getContentWidth() / 2.f, liveToggle->getScaledContentHeight() / 2.f + 7.f});
    liveToggle->setID("preview-toggle"_spr);

    previewMenu->addChild(liveToggle);

    auto specialsMenu = CCMenu::create();
    specialsMenu->ignoreAnchorPointForPosition(false);
    specialsMenu->setAnchorPoint({0.5f, 0.5f});
    specialsMenu->setContentSize({70.f, 125.f});
    specialsMenu->setPosition(size.width / 2.f - 130.f, size.height / 2.f - 7.5f);
    specialsMenu->setZOrder(1);
    specialsMenu->setID("specials-menu"_spr);
    specialsMenu->setLayout(
        RowLayout::create()
            ->setCrossAxisOverflow(false)
            ->setGrowCrossAxis(true)
            ->setAxisAlignment(AxisAlignment::Center)
            ->setCrossAxisAlignment(AxisAlignment::Center)
            ->setGap(4.f)
            ->setAxisReverse(true)
            ->setCrossAxisReverse(true)
    );

    if (!compactUI) {
        specialsMenu->setContentSize({ 125.f, 125.f });
        specialsMenu->setPositionX(size.width / 2.f - 102.5f);
    }

    auto& order = compactUI ? tinker::constants::color_channels::OrderSmall : tinker::constants::color_channels::OrderLarge;

    int i = 0;
    int iOffset = 0;
    for (const auto& channel : order) {
        if (channel == tinker::constants::color_channels::Default) continue;
        auto btn = createChannelButton(channel);
        btn->setZOrder(order.size() - i - 1 - iOffset);
        specialsMenu->addChild(btn);
        if (!compactUI) {
            if (channel == tinker::constants::color_channels::White) {
                iOffset = 1;
            }
        }
        i++;
    }

    if (!compactUI) {
        auto spacer1 = CCNode::create();
        spacer1->setContentSize({28.4f / 2.f, 29.2f});
        spacer1->setID("empty-space-1"_spr);
        spacer1->setZOrder(3);
        specialsMenu->addChild(spacer1);

        auto spacer2 = CCNode::create();
        spacer2->setContentSize({28.4f / 2.f, 29.2f});
        spacer2->setID("empty-space-2"_spr);
        spacer2->setZOrder(-1);
        specialsMenu->addChild(spacer2);
    }

    specialsMenu->updateLayout();

    popup->m_mainLayer->addChild(specialsMenu);

    auto specialTitle = geode::Label::create("Special", "bigFont.fnt");
    specialTitle->setID("special-channels-title"_spr);
    specialTitle->setScale(0.35f);
    specialTitle->setZOrder(1);
    specialTitle->setPosition(
        specialsMenu->getPositionX(),
        specialsMenu->getPositionY() + 72.f
    );
    popup->m_mainLayer->addChild(specialTitle);

    auto bg = geode::NineSlice::create("square02_001.png");
    bg->setOpacity(80);
    bg->setAnchorPoint({0.f, 0.5f});
    bg->setContentSize({340.f - specialsMenu->getContentWidth() - 15.f, 150.f});
    bg->setPosition({specialsMenu->getPositionX() + specialsMenu->getContentWidth() / 2.f + 10.f, size.height / 2.f});
    bg->setID("colors-background"_spr);

    popup->m_mainLayer->addChild(bg);

    auto specialBg = geode::NineSlice::create("square02_001.png");
    specialBg->setOpacity(80);
    specialBg->setContentSize({specialsMenu->getContentWidth() + 10.f, 150.f});
    specialBg->setPosition({specialsMenu->getPositionX(), size.height / 2.f});
    specialBg->setID("special-background"_spr);

    popup->m_mainLayer->addChild(specialBg);

    m_popupData.m_colorList = alpha::ui::AdvancedScrollLayer::create({bg->getContentWidth(), 150.f});
    m_popupData.m_colorList->setAnchorPoint({1.f, 1.f});
    m_popupData.m_colorList->setID("color-list"_spr);
    m_popupData.m_colorList->setZOrder(1);
    m_popupData.m_colorList->setPosition({bg->getPositionX() + bg->getContentWidth(), bg->getPositionY() + bg->getContentHeight() / 2.f});
    #ifdef GEODE_IS_MOBILE
    m_popupData.m_colorList->setScrollDelta(1.5f);
    #else
    m_popupData.m_colorList->setScrollDelta(1.f);
    #endif

    float singleWidth = compactUI ? 21.2f : 28.5f;
    float singleHeight = compactUI ? 22.f : 29.3f;

    m_popupData.m_colorList->setCullingMethod([this, singleHeight] (cocos2d::CCNode* content, const cocos2d::CCPoint& scroll) {
        cull(content, scroll, singleHeight);
    });

    float gap = 3.5f;
    int columnCount = compactUI ? 10 : 6;
    float padding = 7.5f;

    m_popupData.m_colorList->getContentLayer()->setContentSize({m_popupData.m_colorList->getContentWidth(), (singleHeight + gap) * (1000.f / columnCount) + padding * 2.f});

    tinker::ui::LazyColorRow* currentRow = nullptr;
    int yIdx = 0;
    for (int i = 0; i < 999; i++) {
        auto idx = i % columnCount;
        if (idx == 0) {
            if (currentRow) {
                currentRow->setContentWidth(currentRow->getContentWidth() - gap);
            }

            currentRow = tinker::ui::LazyColorRow::create([this, start = i, columnCount, singleWidth, gap, singleHeight] (tinker::ui::LazyColorRow* self) {
                for (int i = start; i < start + columnCount; i++) {
                    if (i >= 999) break;

                    auto idx = i % columnCount;

                    auto button = createChannelButton(i + 1);
                    auto x = singleWidth / 2.f + (singleWidth + gap) * idx;
                    button->setPosition({x, singleHeight / 2.f});

                    self->addChild(button);
                    self->setContentWidth(self->getContentWidth() + singleWidth + gap);
                }
            }, i + 1, i + columnCount);
            currentRow->ignoreAnchorPointForPosition(false);
            currentRow->setContentSize({0.f, singleHeight});
            currentRow->setAnchorPoint({0.5f, 1.f});
            currentRow->setPosition({m_popupData.m_colorList->getContentLayer()->getContentWidth() / 2.f, m_popupData.m_colorList->getContentLayer()->getContentHeight() - (singleHeight + gap) * yIdx - padding});
            currentRow->setID(fmt::format("color-row-{}"_spr, yIdx));

            m_popupData.m_colorList->getContentLayer()->addChild(currentRow);

            m_popupData.m_rows.push_back(currentRow);
            yIdx++;
        }
    }
    if (currentRow) {
        currentRow->setContentWidth(currentRow->getContentWidth() - gap);
    }

    popup->m_mainLayer->addChild(m_popupData.m_colorList);

    auto scrollBar = alpha::ui::AdvancedScrollBar::create(m_popupData.m_colorList, alpha::ui::ScrollOrientation::VERTICAL);
    auto style = alpha::ui::RoundedScrollStyle();
    style.m_track = [] {
        auto track = alpha::ui::RoundedScrollTrack::create();
        track->setClickColor({0, 0, 0, 0});
        track->setBackgroundColor({0, 0, 0, 0});
        return track;
    };

    scrollBar->setStyle(style);
    scrollBar->setContentSize({10.f, m_popupData.m_colorList->getContentHeight() - 10.f});
    scrollBar->setPosition({m_popupData.m_colorList->getPositionX() + scrollBar->getContentWidth() / 2.f - 8.f, scrollBar->getPositionY()});
    scrollBar->setZOrder(100);
    scrollBar->setTouchPriority(m_popupData.m_colorList->getTouchPriority() - 10.f);
    scrollBar->setID("color-list-scrollbar"_spr);

    popup->m_mainLayer->addChild(scrollBar);

    m_popupData.m_channelInput = geode::TextInput::create(50, "Num");
    m_popupData.m_channelInput->setPosition({size.width / 2.f + 210.f, size.height / 2.f});
    m_popupData.m_channelInput->getInputNode()->m_placeholderColor = ccColor3B{120, 170, 240};
    m_popupData.m_channelInput->getInputNode()->refreshLabel();
    m_popupData.m_channelInput->setID("color-input"_spr);
    m_popupData.m_channelInput->setCallback([this] (auto text) {
        auto numRes = geode::utils::numFromString<int>(text);
        if (numRes) {
            m_popupData.m_channel = numRes.unwrap();
            updateChannel(false);
        }
    });
    m_popupData.m_channelInput->setString("1");
    popup->m_mainLayer->addChild(m_popupData.m_channelInput);


    auto rowInput = geode::TextInput::create(50, "Rows");
    rowInput->setScale(0.65f);
    rowInput->setPosition({size.width - rowInput->getScaledContentWidth() / 2.f - 12.5f, rowInput->getScaledContentHeight() / 2.f + 10.f});
    rowInput->getInputNode()->m_placeholderColor = ccColor3B{120, 170, 240};
    rowInput->getInputNode()->refreshLabel();
    rowInput->setID("row-input"_spr);
    rowInput->setString(numToString(m_rows));
    rowInput->setCallback([this] (auto text) {
        auto numRes = geode::utils::numFromString<int>(text);
        if (numRes) {
            m_rows = std::clamp(numRes.unwrap(), 1, 25);
        }
    });

    popup->m_mainLayer->addChild(rowInput);
    
    m_popupData.m_rowLabel = geode::Label::create(m_isHorizontal ? "Cols" : "Rows", "bigFont.fnt");
    m_popupData.m_rowLabel->setScale(0.3f);
    m_popupData.m_rowLabel->setAnchorPoint({0.5f, 0.f});
    m_popupData.m_rowLabel->setPosition(rowInput->getPosition() + CCPoint{0.f, rowInput->getScaledContentHeight() / 2.f + 2.f});
    m_popupData.m_rowLabel->setID("row-label"_spr);

    popup->m_mainLayer->addChild(m_popupData.m_rowLabel);

    auto horizontalToggle = tinker::ui::LabelledToggle::create("Horizontal", [this] (bool state) {
        m_isHorizontal = state;
        m_popupData.m_rowLabel->setText(m_isHorizontal ? "Cols" : "Rows");
    });
    horizontalToggle->setScale(0.9f);
    horizontalToggle->setAnchorPoint({0.f, 0.f});
    horizontalToggle->setPosition(CCPoint{12.5f, 12.5f} / horizontalToggle->getScale());
    horizontalToggle->toggle(m_isHorizontal);
    horizontalToggle->setID("horizontal-toggle"_spr);

    popup->m_mainLayer->addChild(horizontalToggle);

    auto upArrow = CCMenuItemExt::createSpriteExtraWithFrameName("edit_leftBtn_001.png", 1.4f, [this] (auto sender) {
        m_popupData.m_channel++;
        updateChannel(true);
    });
    upArrow->setRotation(90.f);
    upArrow->setPosition({size.width / 2.f + 210.f, m_popupData.m_channelInput->boundingBox().getMaxY() + upArrow->getContentHeight() / 2.f + 4.f});
    upArrow->setID("channel-up-button"_spr);
    RepeatingEditorButtons::setRepeatable(upArrow, true);

    popup->m_buttonMenu->addChild(upArrow);

    auto downArrow = CCMenuItemExt::createSpriteExtraWithFrameName("edit_rightBtn_001.png", 1.4f, [this] (auto sender) {
        m_popupData.m_channel--;
        updateChannel(true);
    });
    downArrow->setRotation(90.f);
    downArrow->setPosition({size.width / 2.f + 210.f, m_popupData.m_channelInput->boundingBox().getMinY() - downArrow->getContentHeight() / 2.f - 4.f});
    downArrow->setID("channel-down-button"_spr);
    RepeatingEditorButtons::setRepeatable(downArrow, true);

    popup->m_buttonMenu->addChild(downArrow);

    auto infoBtn = InfoAlertButton::create("Help", "<cg>View</c> up to 100 color channels live.\n<cp>Preview</c> shows the colors as they are at your editor position.\nYou can set the <cb>Row/Column</c> count for how many color channels show until it separates them.", 0.9f);

    popup->m_buttonMenu->addChildAtPosition(infoBtn, Anchor::TopLeft, {infoBtn->getContentWidth() / 2.f + 7.5f, -infoBtn->getContentHeight() / 2.f - 7.5f}, false);
}

void ColorChannelGizmo::updateChannel(bool updateInput) {
    if (m_popupData.m_channel < 1 || m_popupData.m_channel > 999) {
        m_popupData.m_channel = 1;
    }

    if (updateInput) {
        m_popupData.m_channelInput->setString(numToString(m_popupData.m_channel));
    }
    
    for (const auto& row : m_popupData.m_rows) {
        if (m_popupData.m_channel >= row->getMin() && m_popupData.m_channel <= row->getMax()) {
            row->load();
            m_popupData.m_colorList->setScrollY(m_popupData.m_colorList->getContentLayer()->getContentHeight() - (row->getPositionY() - row->getContentHeight() / 2.f + m_popupData.m_colorList->getContentHeight() / 2.f), true);
        }
    }
}

void ColorChannelGizmo::onEditPopupClose(geode::Popup* popup) {
    m_popupData.m_colorList->removeFromParent();

    m_popupData = {};
    updateColorButtons();
}

void ColorChannelGizmo::cull(cocos2d::CCNode* content, const cocos2d::CCPoint& scroll, float singleHeight) {
    auto scrollYEnd = scroll.y + m_popupData.m_colorList->getContentHeight() + singleHeight;

    for (const auto& row : m_popupData.m_rows) {
        auto y = m_popupData.m_colorList->getContentLayer()->getContentHeight() - row->getPositionY();

        if (scroll.y - singleHeight < y && scrollYEnd >= (y + singleHeight)) {
            row->load();
            if (!row->getParent()) {
                m_popupData.m_colorList->getContentLayer()->addChild(row);
            }
        }
        else {
            if (row->getParent()) {
                row->removeFromParentAndCleanup(false);
            }
        }
    }
}

CCMenuItemSpriteExtra* ColorChannelGizmo::createChannelButton(int channel) {
    using namespace tinker::constants::color_channels;
    bool compactUI = ImprovedColorPicker::getSetting<bool, "compact-ui">();

    auto spr = tinker::ui::ColorChannelSprite::create(channel, true);
    m_popupData.m_colorChannelSprites.push_back(spr);
    
    auto btn = CCMenuItemExt::createSpriteExtra(spr, [this, spr, channel] (auto sender) {
        if (m_colorChannels.size() >= 100 && !spr->isSelected()) {
            createQuickPopup("Too many colors!", "You can only have 100 colors in one Gizmo!", "OK", nullptr, nullptr);
            return;
        }
        spr->setSelected(!spr->isSelected());

        if (spr->isSelected()) {
            m_colorChannels.insert(channel);
        }
        else {
            m_colorChannels.erase(channel);
        }
    });

    btn->setTag(channel);
    btn->setID(fmt::format("channel-{}-button"_spr, channel));
    btn->setScale(compactUI ? 0.746f : 1.f);
    btn->m_baseScale = btn->getScale();

    if (m_colorChannels.contains(channel)) {
        spr->setSelected(true);
    }

    return btn;
}