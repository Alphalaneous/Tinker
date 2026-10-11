#pragma once

#include <Geode/Enums.hpp>
#include <Geode/binding/CCMenuItemToggler.hpp>
#include <Geode/binding/ColorSelectDelegate.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/LevelSettingsLayer.hpp>
#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/cocos/cocoa/CCGeometry.h>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>
#include <Geode/cocos/sprite_nodes/CCSprite.h>
#include <Geode/ui/Button.hpp>
#include <Geode/utils/ZStringView.hpp>
#include <Geode/utils/cocos.hpp>
#include "nodes/ColorChannelSprite.hpp"
#include <alphalaneous.alphas-ui-pack/include/API.hpp>
#include <unordered_map>
#include <vector>
#include "ArtPreview.hpp"

namespace tinker::ui {

class SelectArtLayer : public geode::Popup, public ColorSelectDelegate {
public:
    static SelectArtLayer* create(LevelEditorLayer* editorLayer, LevelSettingsLayer* levelSettingsLayer, SelectArtType type);

    void loadType(SelectArtType type);
    void updateSprites(SelectArtType type);
    void colorSelectClosed(cocos2d::CCNode* popup);

protected:
    bool init(LevelEditorLayer* editorLayer, LevelSettingsLayer* levelSettingsLayer, SelectArtType type);
    geode::Button* createArtButton(SelectArtType type, int id);
    CCMenuItemToggler* createTabToggler(SelectArtType type, geode::ZStringView text, geode::ZStringView id, const cocos2d::CCPoint& offset = {0, 0});
    CCMenuItemToggler* createLineToggler(int type);
    geode::Button* createColorButton(int colorID);
    void updateColors();

    int m_backgroundID = 0;
    int m_groundID = 0;
    int m_groundLineID = 0;
    int m_middlegroundID = 0;

    bool m_backgroundChanged = false;
    bool m_groundChanged = false;
    bool m_middlegroundChanged = false;

    SelectArtType m_type = SelectArtType::Background;

    cocos2d::CCLayerColor* m_scrollBG = nullptr;
    geode::Ref<alpha::ui::AdvancedScrollLayer> m_scrollLayer = nullptr;
    LevelEditorLayer* m_editorLayer = nullptr;
    LevelSettingsLayer* m_levelSettingsLayer = nullptr;
    ArtPreview* m_stylePreview = nullptr;
    SelectArtDelegate* m_delegate = nullptr;
    std::vector<CCMenuItemToggler*> m_tabTogglers{};
    cocos2d::CCSprite* m_topShadow = nullptr;

    cocos2d::CCNode* m_lineContainer = nullptr;
    std::vector<CCMenuItemToggler*> m_lineTogglers{};
    std::vector<tinker::ui::ColorChannelSprite*> m_colorSprites{};

    std::unordered_map<SelectArtType, std::vector<geode::Ref<geode::Button>>> m_artNodes{};
};

}