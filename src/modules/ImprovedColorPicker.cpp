#include "modules/ImprovedColorPicker.hpp"
#include "utils/Constants.hpp"
#include "utils/next-free/NextFreeProvider.hpp"
#include "utils/next-free/sources/ColorSource.hpp"
#include "nodes/NextFreeOffsetInput.hpp"

bool ImprovedColorPicker::onToggled(bool state) {
    m_toggledHooks.toggle(state);
    return true;
}

bool ImprovedColorPicker::onSettingChanged(std::string_view key, const matjson::Value& value) {
    return true;
}

void ICPCustomizeObjectLayer::onSelectMode(CCObject* sender) {
    CustomizeObjectLayer::onSelectMode(sender);
    bool isColorMode = m_selectedMode == 1 || m_selectedMode == 2;
    if (isColorMode) {
        checkAllowLighter();

        m_customColorChannel = getActiveMode(true);
        updateCustomColorLabels();
        scrollToChannel(m_customColorChannel, true);

        if (m_colorSprite) m_colorSprite->setVisible(false);
    }

    m_customColorInput->setVisible(isColorMode);
    m_customColorInputBG->setVisible(isColorMode);

    if (auto menu = m_mainLayer->getChildByID("browse-menu")) {
        if (auto button = menu->getChildByID("browse-button")) {
            button->setVisible(isColorMode);
        }
        menu->updateLayout();
    }

    auto fields = m_fields.self();
    if (fields->m_previewMenu) {
        fields->m_previewMenu->setVisible(isColorMode);
    }
}

void ICPCustomizeObjectLayer::updateLighterButtons() {
    auto fields = m_fields.self();
    for (const auto& btn : fields->m_lighterButtons) {
        btn->setEnabled(fields->m_allowLighterChannel);
        auto spr = static_cast<tinker::ui::ColorChannelSprite*>(btn->getChildByID("channel-sprite"_spr));
        if (!spr) return;

        spr->setEnabled(fields->m_allowLighterChannel);
    }
}

void ICPCustomizeObjectLayer::checkAllowLighter() {
    auto fields = m_fields.self();

    if (m_selectedMode == 1) {
        fields->m_allowLighterChannel = false;
        updateLighterButtons();
        return;
    }
    
    if (m_targetObject) {
        fields->m_allowLighterChannel = m_targetObject->m_baseColor && m_targetObject->m_detailColor;
    }
    else if (m_targetObjects) {
        fields->m_allowLighterChannel = true;
        for (auto obj : m_targetObjects->asExt<GameObject>()) {
            if (!(obj->m_baseColor && obj->m_detailColor)) {
                fields->m_allowLighterChannel = false;
                break;
            }
        }
    }

    updateLighterButtons();
}

void ICPCustomizeObjectLayer::onSelectColor(CCObject* sender) {
    auto channel = sender->getTag();
    if (!channel) {
        m_customColorChannel = 0;
        updateSelected(m_customColorChannel);
        channel = getActiveMode(true);
    }
    m_customColorChannel = channel;

    setChannelModified();
    updateSelected(m_customColorChannel);
    updateColorSprite();
    highlightSelected(nullptr);
    updateCustomColorLabels();
}

void ICPCustomizeObjectLayer::updateSelection(const std::vector<Ref<tinker::ui::ColorChannelSprite>>& items, bool updateColor) {
    auto selected = getActiveMode(true);
    auto fields = m_fields.self();

    for (const auto& row : fields->m_rows) {
        if (selected >= row->getMin() && selected <= row->getMax()) {
            row->load();
        }
    }

    for (const auto& sprite : items) {
        if (updateColor) sprite->updateSprite();
        sprite->setSelected(sprite->getColorID() == selected);
    }

    if (updateColor) {
        if (fields->m_colorSprite) fields->m_colorSprite->setColorID(m_customColorChannel);
    }
}

void ICPCustomizeObjectLayer::highlightSelected(ButtonSprite* sprite) {
    auto fields = m_fields.self();
    updateSelection(fields->m_colorChannelSprites, false);
    updateSelection(fields->m_recentColorSprites, false);
}

void ICPCustomizeObjectLayer::updateColorSprite() {
    CustomizeObjectLayer::updateColorSprite();
    auto fields = m_fields.self();
    updateSelection(fields->m_colorChannelSprites, true);
    updateSelection(fields->m_recentColorSprites, true);
}

void ICPCustomizeObjectLayer::updateLiveSelectButton() {
    using namespace tinker::constants::color_channels;

    auto fields = m_fields.self();

    if (fields->m_inited) {
        switch (m_customColorChannel) {
            case PlayerColor1:
            case PlayerColor2:
            case LightBackground:
            case Lighter:
            case Black:
            case White:
            case Default:
            case -1:
                m_liveSelectButton->setVisible(false);
                if (m_customColorChannel == -1 && ImprovedColorPicker::getSetting<bool, "out-of-range-ids">()) {
                    m_liveSelectButton->setVisible(true);
                }
                break;
            default:
                m_liveSelectButton->setVisible(true);
                break;
        }
        if (fields->m_colorSprite) fields->m_colorSprite->setColorID(m_customColorChannel);
        if (m_colorSprite) m_colorSprite->setVisible(false);
    }

}

void ICPCustomizeObjectLayer::updateCustomColorLabels() {
    using namespace tinker::constants::color_channels;
    auto fields = m_fields.self();

    std::string text;
    if (m_customColorChannel > 0 && m_customColorChannel < 1000) {
        text = utils::numToString(m_customColorChannel);
    }
    else {
        text = GJSpecialColorSelect::textForColorIdx(m_customColorChannel);
    }

    auto delegate = m_customColorInput->m_delegate;
    m_customColorInput->setDelegate(nullptr);
    m_customColorInput->setString(text);
    m_customColorInput->setDelegate(delegate);
    m_customColorInput->m_textField->detachWithIME();

    updateLiveSelectButton();
}

void ICPCustomizeObjectLayer::onUpdateCustomColor(CCObject* sender) {
    auto compactUI = ImprovedColorPicker::getSetting<bool, "compact-ui">();
    auto& order = compactUI ? tinker::constants::color_channels::OrderSmall : tinker::constants::color_channels::OrderLarge;
    auto fields = m_fields.self();

    m_customColorSelected = true;
    int i = m_customColorChannel;
    bool next = sender->getTag() == 1;

    auto it = std::find(order.begin(), order.end(), i);

    if (i < 1000) {
        if (next) {
            i = i < 999 ? i + 1 : order.at(1);
        }
        else {
            i = i > 1 ? i - 1 : order.back();
        }
    }
    else if (it == order.end()) {
        i = 1;
    }
    else if (next) {
        i = std::next(it) == order.end() ? 1 : *std::next(it);
        if (!fields->m_allowLighterChannel && i == tinker::constants::color_channels::Lighter) {
            ++it;
            i = *std::next(it);
        }
    }
    else if (it != order.begin() && i != order.at(1)) {
        i = *std::prev(it);
        if (!fields->m_allowLighterChannel && i == tinker::constants::color_channels::Lighter) {
            --it;
            i = *std::prev(it);
        }
    }
    else if (!next && i == order.at(1)) {
        i = 999;
    }

    m_customColorChannel = i;
    updateCustomColorLabels();
    updateSelected(i);
    setChannelModified();
    m_customColorSelected = false;
    updateColorSprite();
    scrollToChannel(m_customColorChannel, false);
    updateLiveSelectButton();
}

void ICPCustomizeObjectLayer::textChanged(CCTextInputNode* input) {
    if (input->getTag() == 0) {
        if (!ImprovedColorPicker::getSetting<bool, "out-of-range-ids">()) {
            auto numRes = geode::utils::numFromString<int>(input->getString());
            if (numRes) {
                auto num = numRes.unwrap();
                if (num > 999 || num < 1) return;
            }
        }
        CustomizeObjectLayer::textChanged(input);
        m_customColorChannel = getActiveMode(true);
        scrollToChannel(m_customColorChannel, false);
        updateLiveSelectButton();
        return;
    }
    CustomizeObjectLayer::textChanged(input);
}

void ICPCustomizeObjectLayer::onNextColorChannel(cocos2d::CCObject* sender) {
    m_customColorSelected = true;
    m_customColorChannel = NextFreeProvider::get()->nextFree<ColorSource>().unwrapOr(0);
    updateCustomColorLabels();
    updateSelected(m_customColorChannel);
    setChannelModified();
    m_customColorSelected = false;
    updateColorSprite();
    scrollToChannel(m_customColorChannel, false);
}

void ICPCustomizeObjectLayer::setChannelModified() {
    auto fields = m_fields.self();
    if (m_selectedMode == 1) {
        fields->m_modifiedChannel1 = true;
        fields->m_finalChannel1 = m_customColorChannel;
    }
    if (m_selectedMode == 2) {
        fields->m_modifiedChannel2 = true;
        fields->m_finalChannel2 = m_customColorChannel;
    }
}

CCMenuItemSpriteExtra* ICPCustomizeObjectLayer::createChannelButton(int channel, bool recent) {
    using namespace tinker::constants::color_channels;
    auto compactUI = ImprovedColorPicker::getSetting<bool, "compact-ui">();
    auto fields = m_fields.self();

    auto spr = tinker::ui::ColorChannelSprite::create(channel, true);
    if (recent) {
        fields->m_recentColorSprites.push_back(spr);
    }
    else {
        fields->m_colorChannelSprites.push_back(spr);
    }

    auto btn = CCMenuItemExt::createSpriteExtra(spr, [this] (auto sender) {
        onSelectColor(sender);
    });

    btn->setTag(channel);
    btn->setID(fmt::format("channel-{}-button", channel));
    btn->setScale(compactUI ? 0.746f : 1.f);
    btn->m_baseScale = btn->getScale();

    if (recent && channel == -2) {
        btn->setEnabled(false);
    }
    if (channel == Lighter) {
        fields->m_lighterButtons.push_back(btn);
    }
    return btn;
}

std::vector<CCMenuItemSpriteExtra*> ICPCustomizeObjectLayer::getRecents() {
    auto compactUI = ImprovedColorPicker::getSetting<bool, "compact-ui">();
    int columnCount = compactUI ? 10 : 6;
    
    std::vector<CCMenuItemSpriteExtra*> buttons;

    auto saved = alpha::level_storage::getSavedValue<std::vector<int>>(LevelEditorLayer::get(), "improved-color-picker/recents");

    for (int i = 0; i < columnCount; i++) {
        int channel = -2;
        if (i < saved.size()) {
            channel = saved[i];
        }
        buttons.push_back(createChannelButton(channel, true));
    }

    return buttons;
}

bool ICPCustomizeObjectLayer::init(GameObject* obj, CCArray* objs) {
    if (!CustomizeObjectLayer::init(obj, objs)) return false;
    auto fields = m_fields.self();

    auto winSize = CCDirector::get()->getWinSize();
    auto compactUI = ImprovedColorPicker::getSetting<bool, "compact-ui">();

    // fod preferred it this way, he is right
    if (auto menu = m_mainLayer->getChildByID("browse-menu")) {
        menu->setPositionY(winSize.height / 2.f - 50.f);
    }
    if (auto menu = m_mainLayer->getChildByID("copy-paste-menu")) {
        menu->setPositionY(winSize.height / 2.f + 50.f);
    }

    m_colorSprite->setVisible(false);

    fields->m_colorSprite = tinker::ui::ColorChannelSprite::create(0, false);
    fields->m_colorSprite->setPosition(m_colorSprite->getPosition());
    fields->m_colorSprite->setScale(m_colorSprite->getScale());

    m_colorSpriteButton->addChild(fields->m_colorSprite);

    auto channelsMenu = m_mainLayer->getChildByID("channels-menu");
    auto specialsMenu = m_mainLayer->getChildByID("special-channels-menu");

    channelsMenu->setVisible(false);
    m_selectedColorLabel->setVisible(false);

    m_colorTabNodes->removeObject(channelsMenu);
    m_colorTabNodes->removeObject(m_selectedColorLabel);

    m_liveSelectButton->setPosition({136.f, -8.f});

    m_arrowUp->setContentSize({35.f, 30.f});
    m_arrowUp->getNormalImage()->setPosition(m_arrowUp->getContentSize() / 2.f);
    m_arrowUp->setSizeMult(1.f);
    m_arrowUp->setPositionY(m_arrowUp->getPositionY() + 5.f);

    m_arrowDown->setContentSize({35.f, 30.f});
    m_arrowDown->getNormalImage()->setPosition(m_arrowDown->getContentSize() / 2.f);
    m_arrowDown->setSizeMult(1.f);
    m_arrowDown->setPositionY(m_arrowDown->getPositionY() - 5.f);

    m_customColorInputBG->setOpacity(120);

    m_colorButtons->removeAllObjects();
    channelsMenu->removeAllChildren();
    specialsMenu->removeAllChildren();

    if (auto menu = m_mainLayer->getChildByID("selected-channel-menu")) {
        menu->setPosition(winSize.width / 2 + 125.f, winSize.height / 2.f - 95.f);
        menu->setScale(0.8f);
    }

    auto selectPos = CCPoint(winSize.width / 2.f + 210.f, winSize.height / 2.f + 5.f);
    
    m_mainLayer->getChildByID("select-channel-menu")->setPosition(selectPos);
    m_mainLayer->getChildByID("channel-input-bg")->setPosition(selectPos);
    m_mainLayer->getChildByID("channel-input")->setPosition(selectPos);
    m_customColorInput->m_maxLabelWidth = 38.f;

    fields->m_previewMenu = CCMenu::create();
    fields->m_previewMenu->setContentSize({35.f, 40.f});
    fields->m_previewMenu->ignoreAnchorPointForPosition(false);
    fields->m_previewMenu->setAnchorPoint({0.5f, 0.5f});
    fields->m_previewMenu->setPosition({winSize.width / 2.f + 210.f, winSize.height / 2.f - 70.f});
    fields->m_previewMenu->setID("preview-menu"_spr);
    m_mainLayer->addChild(fields->m_previewMenu);

    auto liveLabel = CCLabelBMFont::create("Preview", "goldFont.fnt");
    liveLabel->setScale(0.3f);
    liveLabel->setAnchorPoint({0.5f, 0.f});
    liveLabel->setPosition({fields->m_previewMenu->getContentWidth() / 2.f, 32.f});
    liveLabel->setID("preview-label"_spr);

    fields->m_previewMenu->addChild(liveLabel);

    auto liveToggle = CCMenuItemExt::createTogglerWithStandardSprites(0.7f, [fields, this] (CCMenuItemToggler* toggler) {
        for (const auto& spr : fields->m_colorChannelSprites) {
            spr->setLive(!toggler->isToggled());
            spr->updateSprite();
        }
        for (const auto& spr : fields->m_recentColorSprites) {
            spr->setLive(!toggler->isToggled());
            spr->updateSprite();
        }

        fields->m_colorSprite->setLive(!toggler->isToggled());
        fields->m_colorSprite->updateSprite();
    });

    liveToggle->setPosition({fields->m_previewMenu->getContentWidth() / 2.f, liveToggle->getScaledContentHeight() / 2.f + 7.f});
    liveToggle->setID("preview-toggle"_spr);

    fields->m_previewMenu->addChild(liveToggle);

    float heightOffset = compactUI ? 35.f : 42.f;

    specialsMenu->setContentSize({70.f, 125.f});
    specialsMenu->setPosition(winSize.width / 2.f - 130.f, winSize.height / 2.f - 7.5f);
    specialsMenu->setLayout(
        RowLayout::create()
            ->setCrossAxisOverflow(false)
            ->setGrowCrossAxis(true)
            ->setAxisAlignment(AxisAlignment::Center)
            ->setCrossAxisAlignment(AxisAlignment::Center)
            ->setGap(4.f)
    );

    if (!compactUI) {
        specialsMenu->setContentSize({ 125.f, 125.f });
        specialsMenu->setPositionX(winSize.width / 2.f - 102.5f);
    }

    auto bg = geode::NineSlice::create("square02_001.png");
    bg->setOpacity(80);
    bg->setAnchorPoint({0.f, 0.5f});
    bg->setContentSize({340.f - specialsMenu->getContentWidth() - 15.f, 150.f - heightOffset});
    bg->setPosition({specialsMenu->getPositionX() + specialsMenu->getContentWidth() / 2.f + 10.f, winSize.height / 2.f + heightOffset / 2.f});
    bg->setID("background"_spr);

    m_mainLayer->addChild(bg);
    m_colorTabNodes->addObject(bg);

    auto specialBg = geode::NineSlice::create("square02_001.png");
    specialBg->setOpacity(80);
    specialBg->setContentSize({specialsMenu->getContentWidth() + 10.f, 150.f});
    specialBg->setPosition({specialsMenu->getPositionX(), winSize.height / 2.f});
    specialBg->setID("special-background"_spr);

    m_mainLayer->addChild(specialBg);
    m_colorTabNodes->addObject(specialBg);

    auto recentsBg = geode::NineSlice::create("square02_001.png");
    recentsBg->setOpacity(80);
    recentsBg->setAnchorPoint({0.f, 0.f});
    recentsBg->setContentSize({bg->getContentWidth(), heightOffset - 5.f});
    recentsBg->setPosition({specialsMenu->getPositionX() + specialsMenu->getContentWidth() / 2.f + 10.f, specialsMenu->getPositionY() - specialsMenu->getContentHeight() / 2.f - 5.f});
    recentsBg->setID("recents-background"_spr);

    m_mainLayer->addChild(recentsBg);
    m_colorTabNodes->addObject(recentsBg);

    fields->m_recentColorsMenu = CCMenu::create();
    fields->m_recentColorsMenu->setAnchorPoint({0.f, 0.f});
    fields->m_recentColorsMenu->ignoreAnchorPointForPosition(false);
    fields->m_recentColorsMenu->setContentSize(recentsBg->getContentSize());
    fields->m_recentColorsMenu->setPosition(recentsBg->getPosition());
    fields->m_recentColorsMenu->setID("recents-menu"_spr);
    fields->m_recentColorsMenu->setLayout(
        SimpleRowLayout::create()
            ->setMainAxisAlignment(MainAxisAlignment::Start)
            ->setCrossAxisAlignment(CrossAxisAlignment::Center)
            ->setGap(3.5f)
            ->setPadding({5.5f, 0.f, 5.5f, 0.f})
    );

    auto buttons = getRecents();
    for (auto btn : buttons) {
        fields->m_recentColorsMenu->addChild(btn);
    }

    fields->m_recentColorsMenu->updateLayout();

    m_mainLayer->addChild(fields->m_recentColorsMenu);
    m_colorTabNodes->addObject(fields->m_recentColorsMenu);

    auto& order = compactUI ? tinker::constants::color_channels::OrderSmall : tinker::constants::color_channels::OrderLarge;

    for (const auto& channel : order) {
        specialsMenu->addChild(createChannelButton(channel));
    }
    specialsMenu->updateLayout();

    auto specialTitle = CCLabelBMFont::create("Special", "bigFont.fnt");
    specialTitle->setID("special-channels-title"_spr);
    specialTitle->setScale(0.35f);
    specialTitle->setPosition(
        specialsMenu->getPositionX(),
        specialsMenu->getPositionY() + 72.f
    );
    m_mainLayer->addChild(specialTitle);
    m_colorTabNodes->addObject(specialTitle);

    fields->m_colorList = alpha::ui::AdvancedScrollLayer::create({bg->getContentWidth(), 150.f - heightOffset});
    fields->m_colorList->setAnchorPoint({1.f, 1.f});
    fields->m_colorList->setID("color-list"_spr);
    fields->m_colorList->setPosition({bg->getPositionX() + bg->getContentWidth(), bg->getPositionY() + bg->getContentHeight() / 2.f});
    #ifdef GEODE_IS_MOBILE
    fields->m_colorList->setScrollDelta(1.5f);
    #else
    fields->m_colorList->setScrollDelta(1.f);
    #endif

    float singleWidth = compactUI ? 21.2f : 28.5f;
    float singleHeight = compactUI ? 22.f : 29.3f;

    fields->m_colorList->setCullingMethod([this, singleHeight] (cocos2d::CCNode* content, const cocos2d::CCPoint& scroll) {
        cull(content, scroll, singleHeight);
    });

    float gap = 3.5f;
    int columnCount = compactUI ? 10 : 6;
    float padding = 7.5f;

    fields->m_colorList->getContentLayer()->setContentSize({fields->m_colorList->getContentWidth(), (singleHeight + gap) * (1000.f / columnCount) + padding * 2.f});

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
            currentRow->setPosition({fields->m_colorList->getContentLayer()->getContentWidth() / 2.f, fields->m_colorList->getContentLayer()->getContentHeight() - (singleHeight + gap) * yIdx - padding});

            fields->m_colorList->getContentLayer()->addChild(currentRow);

            fields->m_rows.push_back(currentRow);
            yIdx++;
        }
    }
    if (currentRow) {
        currentRow->setContentWidth(currentRow->getContentWidth() - gap);
    }

    m_mainLayer->addChild(fields->m_colorList);
    m_colorTabNodes->addObject(fields->m_colorList);

    auto scrollBar = alpha::ui::AdvancedScrollBar::create(fields->m_colorList, alpha::ui::ScrollOrientation::VERTICAL);
    auto style = alpha::ui::RoundedScrollStyle();
    style.m_track = [] {
        auto track = alpha::ui::RoundedScrollTrack::create();
        track->setClickColor({0, 0, 0, 0});
        track->setBackgroundColor({0, 0, 0, 0});
        return track;
    };

    scrollBar->setStyle(style);
    scrollBar->setContentSize({10.f, fields->m_colorList->getContentHeight() - 10.f});
    scrollBar->setPosition({fields->m_colorList->getPositionX() + scrollBar->getContentWidth() / 2.f - 8.f, scrollBar->getPositionY()});
    scrollBar->setZOrder(100);
    scrollBar->setTouchPriority(fields->m_colorList->getTouchPriority() - 10.f);
    scrollBar->setID("color-list-scrollbar"_spr);

    m_mainLayer->addChild(scrollBar);
    m_colorTabNodes->addObject(scrollBar);

    if (auto nextFreeMenu = m_mainLayer->getChildByID("next-free-menu")) {
        auto nextFreeOffset = tinker::ui::NextFreeOffsetInput<ColorSource>::create();
        nextFreeOffset->setID("next-free-offset-input"_spr);
        nextFreeMenu->addChild(nextFreeOffset);
        nextFreeMenu->updateLayout();
    }

    checkAllowLighter();

    m_customColorChannel = getActiveMode(true);
    highlightSelected(nullptr);
    updateCustomColorLabels();
    scrollToChannel(m_customColorChannel, true);

    if (m_customColorChannel == tinker::constants::color_channels::Default) {
        m_liveSelectButton->setVisible(false);
    }

    cull(fields->m_colorList->getContentLayer(), fields->m_colorList->getScrollPoint(), singleHeight);
    if (fields->m_colorSprite) fields->m_colorSprite->setColorID(m_customColorChannel);

    fields->m_inited = true;
    
    return true;
}

void ICPCustomizeObjectLayer::cull(cocos2d::CCNode* content, const cocos2d::CCPoint& scroll, float singleHeight) {
    auto fields = m_fields.self();
    auto scrollYEnd = scroll.y + fields->m_colorList->getContentHeight() + singleHeight;

    for (const auto& row : fields->m_rows) {
        auto y = fields->m_colorList->getContentLayer()->getContentHeight() - row->getPositionY();

        if (scroll.y - singleHeight < y && scrollYEnd >= (y + singleHeight)) {
            row->load();
            if (!row->getParent()) {
                fields->m_colorList->getContentLayer()->addChild(row);
            }
        }
        else {
            if (row->getParent()) {
                row->removeFromParentAndCleanup(false);
            }
        }
    }
}

void ICPCustomizeObjectLayer::onClose(cocos2d::CCObject* sender) {
    auto fields = m_fields.self();

    auto saved = alpha::level_storage::getSavedValue<std::vector<int>>(
        LevelEditorLayer::get(),
        "improved-color-picker/recents"
    );

    std::vector<int> recents;
    std::set<int> recentsSet;

    recents.reserve(10);

    if (fields->m_modifiedChannel1) {
        if (fields->m_finalChannel1 != 0) {
            recents.push_back(fields->m_finalChannel1);
            recentsSet.insert(fields->m_finalChannel1);
        }
    }

    if (fields->m_modifiedChannel2 && fields->m_finalChannel1 != fields->m_finalChannel2) {
        if (fields->m_finalChannel2 != 0) {
            recents.push_back(fields->m_finalChannel2);
            recentsSet.insert(fields->m_finalChannel2);
        }
    }

    for (int channel : saved) {
        if (fields->m_modifiedChannel1 && channel == fields->m_finalChannel1) continue;
        if (fields->m_modifiedChannel2 && channel == fields->m_finalChannel2) continue;
        if (channel == 0) continue;
        if (recentsSet.contains(channel)) continue;

        recents.push_back(channel);
        recentsSet.insert(channel);
        if (recents.size() == 10) {
            break;
        }
    }

    alpha::level_storage::setSavedValue(
        LevelEditorLayer::get(),
        "improved-color-picker/recents",
        recents
    );

    fields->m_colorList->removeFromParent();

    CustomizeObjectLayer::onClose(sender);
}

void ICPCustomizeObjectLayer::scrollToChannel(int channel, bool instant) {
    if (channel < 1 || channel > 999) {
        channel = 1;
    }

    auto fields = m_fields.self();
    for (const auto& row : fields->m_rows) {
        if (channel >= row->getMin() && channel <= row->getMax()) {
            row->load();
            fields->m_colorList->setScrollY(fields->m_colorList->getContentLayer()->getContentHeight() - (row->getPositionY() - row->getContentHeight() / 2.f + fields->m_colorList->getContentHeight() / 2.f), !instant);
        }
    }
}