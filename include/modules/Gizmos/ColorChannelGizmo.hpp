#pragma once

#include "modules/Gizmos/Gizmos.hpp"
#include "nodes/ColorVisualButton.hpp"
#include "nodes/LazyColorRow.hpp"

class ColorChannelGizmo : public GizmoInterface {
public:
    cocos2d::CCNode* setup() override;
    void onEditObject() override;
    void loadData(std::optional<const matjson::Value> data) override;
    matjson::Value saveData() override;
    void onRotate(float degrees) override;
    void update(float dt) override;
    void setupEditPopup(geode::Popup* popup) override;
    void onEditPopupClose(geode::Popup* popup) override;

protected:
    std::vector<cocos2d::CCMenu*> m_colorsMenus{};
    geode::NineSlice* m_background = nullptr;

    std::set<int> m_colorChannels{};
    std::vector<tinker::ui::ColorVisualButton*> m_colorButtons{};
    int m_rows = 10;
    bool m_isHorizontal = false;

    void updateChannel(bool updateInput);
    void updateColorButtons();

    void cull(cocos2d::CCNode* content, const cocos2d::CCPoint& scroll, float singleHeight);
    CCMenuItemSpriteExtra* createChannelButton(int channel);

    struct PopupData {
        std::vector<geode::Ref<tinker::ui::ColorChannelSprite>> m_colorChannelSprites{};
        alpha::ui::AdvancedScrollLayer* m_colorList = nullptr;
        std::vector<geode::Ref<tinker::ui::LazyColorRow>> m_rows{};
        geode::Label* m_rowLabel = nullptr;
        geode::TextInput* m_channelInput = nullptr;
        int m_channel = 1;
    } m_popupData{};
};