#pragma once

#include <alphalaneous.alphas-ui-pack/include/API.hpp>
#include <alphalaneous.level-storage-api/include/LevelStorageAPI.hpp>
#include "module/Module.hpp"
#include <Geode/modify/CustomizeObjectLayer.hpp>
#include <Geode/modify/ColorChannelSprite.hpp>

class $editorModule(ImprovedColorPicker) {
    bool onToggled(bool state) override;
    bool onSettingChanged(std::string_view key, const matjson::Value& value) override;

    Ref<CCSpriteFrame> m_colorChannelSpriteFrame;

    tinker::utils::ScopedHookToggle<"hjfod.betteredit", ImprovedColorPicker> m_toggledHooks = {
        "CustomizeObjectLayer::onSelectColor",
        "CustomizeObjectLayer::highlightSelected",
        "CustomizeObjectLayer::updateColorSprite",
        "CustomizeObjectLayer::updateCustomColorLabels",
        "CustomizeObjectLayer::onUpdateCustomColor",
        "CustomizeObjectLayer::textChanged",
        "CustomizeObjectLayer::init",
        "CustomizeObjectLayer::onNextColorChannel"
    };
};

namespace tinker::ui {

class LazyColorRow : public CCMenu {
public:
    static LazyColorRow* create(geode::Function<void(LazyColorRow* self)> callback, int min, int max);
    bool init(geode::Function<void(LazyColorRow* self)> callback, int min, int max);

    int getMin();
    int getMax();

    void load();
protected:
    geode::Function<void(LazyColorRow* self)> m_callback;
    bool m_loaded;
    int m_min;
    int m_max;
};

}

class CheatColorChannelSprite : public CCSprite {
public:
    bool init();

    cocos2d::CCLabelBMFont* m_copyLabel;
    cocos2d::CCLabelBMFont* m_opacityLabel;
    cocos2d::CCLabelBMFont* m_blendingLabel;
};

class $modify(ICPColorChannelSprite, ColorChannelSprite) {
    $registerEditorHooks(ImprovedColorPicker)
    
    static ColorChannelSprite* create();
};

class $modify(ICPCustomizeObjectLayer, CustomizeObjectLayer) {
    $registerEditorHooks(ImprovedColorPicker)

    struct Fields {
        std::vector<Ref<ColorChannelSprite>> m_colorChannelSprites;
        alpha::ui::AdvancedScrollLayer* m_colorList;
        std::vector<Ref<tinker::ui::LazyColorRow>> m_rows;

        std::vector<Ref<ColorChannelSprite>> m_recentColorSprites;
        CCMenu* m_recentColorsMenu;

        std::vector<Ref<CCMenuItemSpriteExtra>> m_lighterButtons;
        Ref<CCSpriteFrame> m_colorChannelSelectionSpriteFrame;
        
        bool m_inited;
        bool m_allowLighterChannel;

        bool m_modifiedChannel1;
        bool m_modifiedChannel2;

        int m_finalChannel1 = -1;
        int m_finalChannel2 = -1;
    };

    void updateSprite(ColorChannelSprite* sprite);
    ColorChannelSprite* createSprite(int channel, bool recent);
    CCMenuItemSpriteExtra* createChannelButton(int channel, bool recent = false);

    std::vector<CCMenuItemSpriteExtra*> getRecents();

    void cull(cocos2d::CCNode* content, const cocos2d::CCPoint& scroll, float singleHeight);
    void updateLiveSelectButton();
    void updateLighterButtons();
    void checkAllowLighter();
    void setChannelModified();
    void updateSelection(const std::vector<Ref<ColorChannelSprite>>& items, bool updateColor);
    void scrollToChannel(int channel, bool instant);
    void onSelectMode(CCObject* sender);
    void onSelectColor(CCObject* sender);
    void highlightSelected(ButtonSprite* sprite);
    void updateColorSprite();
    void updateCustomColorLabels();
    void onUpdateCustomColor(CCObject* sender);
    void onNextColorChannel(cocos2d::CCObject* sender);
    void textChanged(CCTextInputNode* input);
    bool init(GameObject* obj, CCArray* objs);
    void onClose(cocos2d::CCObject* sender);
};