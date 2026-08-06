#include "ImprovedColorPicker.hpp"
#include "utils/Constants.hpp"
#include "utils/NextFree/NextFreeProvider.hpp"
#include "utils/NextFree/sources/ColorSource.hpp"
#include "utils/NextFree/NextFreeOffsetInput.hpp"

bool ImprovedColorPicker::onToggled(bool state) {
    m_toggledHooks.toggle(state);
    return true;
}

bool ImprovedColorPicker::onSettingChanged(std::string_view key, const matjson::Value& value) {
    return true;
}

namespace tinker::ui {

LazyColorRow* LazyColorRow::create(geode::Function<void(LazyColorRow* self)> callback, int min, int max) {
    auto ret = new LazyColorRow();
    if (ret->init(std::move(callback), min, max)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool LazyColorRow::init(geode::Function<void(LazyColorRow* self)> callback, int min, int max) {
    if (!CCMenu::init()) return false;
    m_callback = std::move(callback);
    m_min = min;
    m_max = max;

    return true;
}

int LazyColorRow::getMin() {
    return m_min;
}

int LazyColorRow::getMax() {
    return m_max;
}

void LazyColorRow::load() {
    if (m_loaded) return;
    if (m_callback) m_callback(this);
    m_loaded = true;
}

}

void ICPCustomizeObjectLayer::onSelectMode(CCObject* sender) {
    CustomizeObjectLayer::onSelectMode(sender);
    checkAllowLighter();

    m_customColorChannel = getActiveMode(true);
    updateCustomColorLabels();
    scrollToChannel(m_customColorChannel, true);
}

void ICPCustomizeObjectLayer::updateLighterButtons() {
    auto fields = m_fields.self();
    for (const auto& btn : fields->m_lighterButtons) {
        btn->setEnabled(fields->m_allowLighterChannel);
        auto spr = static_cast<CCSprite*>(btn->getChildByID("channel-sprite"_spr));
        if (!spr) return;

        auto opacity = fields->m_allowLighterChannel ? 255 : 50;
        spr->setOpacity(opacity);

        auto label = static_cast<CCLabelBMFont*>(spr->getChildByID("id-label"_spr));
        if (!label) return;
        label->setOpacity(opacity);
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

void ICPCustomizeObjectLayer::updateSelection(const std::vector<Ref<ColorChannelSprite>>& items, bool updateColor) {
    auto selected = getActiveMode(true);
    auto fields = m_fields.self();

    for (const auto& row : fields->m_rows) {
        if (selected >= row->getMin() && selected <= row->getMax()) {
            row->load();
        }
    }

    for (const auto& sprite : items) {
        if (auto i = sprite->getChildByID("selected-indicator"_spr)) {
            auto channelObj = typeinfo_cast<CCInteger*>(sprite->getUserObject("channel"_spr));
            if (!channelObj) continue;
            if (updateColor) updateSprite(sprite);
            i->setVisible(channelObj->getValue() == selected);
        }
    }

    if (updateColor) {
        updateSprite(m_colorSprite, m_customColorChannel);
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
            case -1:
                m_liveSelectButton->setVisible(false);
                if (m_customColorChannel == -1 && ImprovedColorPicker::getSetting<bool, "out-of-range-ids">()) {
                    m_liveSelectButton->setVisible(true);
                }
                m_colorSprite->setVisible(false);
                break;
            default:
                m_liveSelectButton->setVisible(true);
                m_colorSprite->setVisible(true);
                break;
        }
        updateSprite(m_colorSprite, m_customColorChannel);
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

ccColor3B ImprovedColorPicker::getRealizedColor(int channelID, unsigned int depth) {
    auto channel = m_editorLayer->m_levelSettings->m_effectManager->getColorAction(channelID);
    if (!channel) {
        return ccWHITE;
    }
    if (channel->m_copyColorLoop || depth > 10) {
        return ccWHITE;
    }
    if (channel->m_copyID) {
        return GameToolbox::transformColor(getRealizedColor(channel->m_copyID, depth + 1), channel->m_copyHSV);
    }
    return channel->m_fromColor;
}

void ICPCustomizeObjectLayer::updateSprite(ColorChannelSprite* sprite, int colorID) {
    auto channelObj = typeinfo_cast<CCInteger*>(sprite->getUserObject("channel"_spr));

    if (colorID == -1) {
        if (!channelObj) return;
        colorID = channelObj->getValue();
    }

    using namespace tinker::constants::color_channels;

    if (colorID == 0) {
        sprite->setOpacity(50);
        return;
    }

    ColorAction* action = nullptr;

    auto allActions = LevelEditorLayer::get()->m_levelSettings->m_effectManager->getAllColorActions();
    for (auto actionI : allActions->asExt<ColorAction>()) {
        if (actionI->m_colorID == colorID) {
            action = actionI;
            break;
        }
    }

    if (action) {
        sprite->updateValues(action);
        if (ImprovedColorPicker::getSetting<bool, "show-copy-color">()) {
            if (action->m_copyID != 0) {
                sprite->setColor(ImprovedColorPicker::get()->getRealizedColor(action->m_colorID));
            }
        }
    }

    if (sprite->m_copyLabel) {
        sprite->m_copyLabel->setScale(0.3f);
        sprite->m_copyLabel->setAnchorPoint({1.f, 1.f});
        sprite->m_copyLabel->setPosition(sprite->getContentSize() - CCPoint{3.5f, 2.5f});
    }

    switch (colorID) {
        case Black: {
            sprite->setColor({0, 0, 0});
            break;
        }
        case LightBackground: {
            sprite->setOpacity(120);
            break;
        }
    }

    if (!channelObj) {
        sprite->setOpacity(255);
    }
}

ColorChannelSprite* ICPCustomizeObjectLayer::createSprite(int channel, bool recent) {
    using namespace tinker::constants::color_channels;

    auto fields = m_fields.self();

    auto spr = ColorChannelSprite::create();
    spr->setScale(0.8f);
    spr->setUserObject("channel"_spr, CCInteger::create(channel));
    spr->setID("channel-sprite"_spr);
    if (!recent) {
        fields->m_colorChannelSprites.push_back(spr);
    }
    else {
        fields->m_recentColorSprites.push_back(spr);
    }

    if (recent && channel == Default) {
        spr->setOpacity(105);
        spr->setColor({20, 20, 20});
    }
    else {
        if (!fields->m_colorChannelSelectionSpriteFrame) {
            fields->m_colorChannelSelectionSpriteFrame = CCSpriteFrameCache::get()->spriteFrameByName("GJ_select_001.png");
        }
        auto selection = CCSprite::createWithSpriteFrame(fields->m_colorChannelSelectionSpriteFrame);
        selection->setScale(1.1f);
        selection->setID("selected-indicator"_spr);
        selection->setPosition(spr->getContentSize() / 2.f);
        selection->setVisible(false);
        spr->addChild(selection);

        if (channel != Black && channel != White) {
            std::string text;

            auto iter = ColorNamesShort.find(channel);
            if (iter == ColorNamesShort.end()) {
                text = utils::numToString(channel);
            }
            else {
                text = iter->second;
            }

            auto label = CCLabelBMFont::create(
                text.c_str(),
                "bigFont.fnt"
            );
            label->limitLabelWidth(25.f, 0.4f, 0.2f);
            label->setPosition(spr->getContentSize() / 2.f + CCPoint{0.f, 0.5f});
            label->setID("id-label"_spr);
            spr->addChild(label);

            if (!fields->m_allowLighterChannel && channel == Lighter) {
                spr->setOpacity(50);
                label->setOpacity(50);
            }
        }
        
        updateSprite(spr);
    }

    return spr;
}

CCMenuItemSpriteExtra* ICPCustomizeObjectLayer::createChannelButton(int channel, bool recent) {
    using namespace tinker::constants::color_channels;
    auto compactUI = ImprovedColorPicker::getSetting<bool, "compact-ui">();

    auto btn = CCMenuItemExt::createSpriteExtra(createSprite(channel, recent), [this] (auto sender) {
        onSelectColor(sender);
    });

    btn->setTag(channel);
    btn->setID(fmt::format("channel-{}-button", channel));
    btn->setScale(compactUI ? 0.746f : 1.f);
    btn->m_baseScale = btn->getScale();

    if (recent && channel == Default) {
        btn->setEnabled(false);
    }
    if (channel == Lighter) {
        m_fields->m_lighterButtons.push_back(btn);
    }
    return btn;
}

std::vector<CCMenuItemSpriteExtra*> ICPCustomizeObjectLayer::getRecents() {
    auto compactUI = ImprovedColorPicker::getSetting<bool, "compact-ui">();
    int columnCount = compactUI ? 10 : 6;
    
    std::vector<CCMenuItemSpriteExtra*> buttons;

    auto saved = alpha::level_storage::getSavedValue<std::vector<int>>(LevelEditorLayer::get(), "improved-color-picker/recents");

    for (int i = 0; i < columnCount; i++) {
        int channel = 0;
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

    m_arrowDown->setContentSize({35.f, 30.f});
    m_arrowDown->getNormalImage()->setPosition(m_arrowDown->getContentSize() / 2.f);
    m_arrowDown->setSizeMult(1.f);

    m_customColorInputBG->setOpacity(120);

    m_colorButtons->removeAllObjects();
    channelsMenu->removeAllChildren();
    specialsMenu->removeAllChildren();

    if (auto menu = m_mainLayer->getChildByID("selected-channel-menu")) {
        menu->setPosition(winSize.width / 2 + 125.f, winSize.height / 2.f - 95.f);
        menu->setScale(0.8f);
    }

    auto selectPos = CCPoint(winSize.width / 2.f + 210.f, winSize.height / 2.f - 15.f);
    m_mainLayer->getChildByID("select-channel-menu")->setPosition(selectPos);
    m_mainLayer->getChildByID("channel-input-bg")->setPosition(selectPos);
    m_mainLayer->getChildByID("channel-input")->setPosition(selectPos);
    m_customColorInput->m_maxLabelWidth = 38.f;

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
        /*auto button = createChannelButton(i + 1);
        auto x = singleWidth / 2.f + (singleWidth + gap) * idx;
        button->setPosition({x, singleHeight / 2.f});

        currentRow->addChild(button);
        currentRow->setContentWidth(currentRow->getContentWidth() + singleWidth + gap);*/
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

    cull(fields->m_colorList->getContentLayer(), fields->m_colorList->getScrollPoint(), singleHeight);
    updateSprite(m_colorSprite, m_customColorChannel);

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
    recents.reserve(10);

    if (fields->m_modifiedChannel1) {
        recents.push_back(fields->m_finalChannel1);
    }

    if (fields->m_modifiedChannel2) {
        recents.push_back(fields->m_finalChannel2);
    }

    for (int channel : saved) {
        if (fields->m_modifiedChannel1 && channel == fields->m_finalChannel1) continue;
        if (fields->m_modifiedChannel2 && channel == fields->m_finalChannel2) continue;

        recents.push_back(channel);
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

// slight performance boost loading 1000+ of the same sprite
bool CheatColorChannelSprite::init() {
    auto& frame = ImprovedColorPicker::get()->m_colorChannelSpriteFrame;
    if (!frame) {
        frame = CCSpriteFrameCache::get()->spriteFrameByName("GJ_colorBtn_001.png");
    }

    return initWithSpriteFrame(frame);
}

ColorChannelSprite* ICPColorChannelSprite::create() {
    auto ret = new CheatColorChannelSprite();
    if (ret->init()) {
        ret->autorelease();
        return reinterpret_cast<ColorChannelSprite*>(ret);
    }
    delete ret;
    return nullptr;
}