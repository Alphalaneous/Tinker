#pragma once

#include "module/Module.hpp"
#include "nodes/ColorChannelSprite.hpp"
#include "nodes/LazyColorRow.hpp"
#include <Geode/modify/CustomizeObjectLayer.hpp>
#include <Geode/modify/ColorChannelSprite.hpp>
#include <alphalaneous.alphas-ui-pack/include/API.hpp>
#include <alphalaneous.level-storage-api/include/LevelStorageAPI.hpp>

class $module(ImprovedColorPicker) {
    bool onToggled(bool state);
    bool onSettingChanged(std::string_view key, const matjson::Value& value);
};

class $modify(ICPCustomizeObjectLayer, CustomizeObjectLayer) {
    $registerHooks(ImprovedColorPicker)

    struct Fields {
        std::vector<geode::Ref<tinker::ui::ColorChannelSprite>> m_colorChannelSprites{};
        alpha::ui::AdvancedScrollLayer* m_colorList = nullptr;
        std::vector<geode::Ref<tinker::ui::LazyColorRow>> m_rows{};

        std::vector<geode::Ref<tinker::ui::ColorChannelSprite>> m_recentColorSprites{};
        cocos2d::CCMenu* m_recentColorsMenu = nullptr;
        cocos2d::CCMenu* m_previewMenu = nullptr;

        std::vector<geode::Ref<CCMenuItemSpriteExtra>> m_lighterButtons{};

        geode::Ref<tinker::ui::ColorChannelSprite> m_colorSprite = nullptr;
        
        bool m_inited = false;
        bool m_allowLighterChannel = false;

        bool m_modifiedChannel1 = false;
        bool m_modifiedChannel2 = false;

        int m_finalChannel1 = -1;
        int m_finalChannel2 = -1;
    };

    CCMenuItemSpriteExtra* createChannelButton(int channel, bool recent = false);

    std::vector<CCMenuItemSpriteExtra*> getRecents();

    void cull(cocos2d::CCNode* content, const cocos2d::CCPoint& scroll, float singleHeight);
    void updateLiveSelectButton();
    void updateLighterButtons();
    void checkAllowLighter();
    void setChannelModified();
    void updateSelection(const std::vector<geode::Ref<tinker::ui::ColorChannelSprite>>& items, bool updateColor);
    void scrollToChannel(int channel, bool instant);
    void onSelectMode(cocos2d::CCObject* sender);
    void onSelectColor(cocos2d::CCObject* sender);
    void highlightSelected(ButtonSprite* sprite);
    void updateColorSprite();
    void updateCustomColorLabels();
    void onUpdateCustomColor(cocos2d::CCObject* sender);
    void onNextColorChannel(cocos2d::CCObject* sender);
    void textChanged(CCTextInputNode* input);
    bool init(GameObject* obj, cocos2d::CCArray* objs);
    void onClose(cocos2d::CCObject* sender);
};