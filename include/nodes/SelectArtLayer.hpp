#pragma once

#include <Geode/Geode.hpp>
#include <alphalaneous.alphas-ui-pack/include/API.hpp>
#include "ArtPreview.hpp"
#include "nodes/ColorChannelSprite.hpp"

using namespace geode::prelude;

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
    CCMenuItemToggler* createTabToggler(SelectArtType type, ZStringView text, ZStringView id, const CCPoint& offset = {0, 0});
    CCMenuItemToggler* createLineToggler(int type);
    geode::Button* createColorButton(int colorID);
    void updateColors();

    int m_backgroundID;
    int m_groundID;
    int m_groundLineID;
    int m_middlegroundID;

    bool m_backgroundChanged;
    bool m_groundChanged;
    bool m_middlegroundChanged;

    SelectArtType m_type;

    CCLayerColor* m_scrollBG;
    Ref<alpha::ui::AdvancedScrollLayer> m_scrollLayer;
    LevelEditorLayer* m_editorLayer;
    LevelSettingsLayer* m_levelSettingsLayer;
    ArtPreview* m_stylePreview;
    SelectArtDelegate* m_delegate;
    std::vector<CCMenuItemToggler*> m_tabTogglers;
    CCSprite* m_topShadow;

    CCNode* m_lineContainer;
    std::vector<CCMenuItemToggler*> m_lineTogglers;
    std::vector<tinker::ui::ColorChannelSprite*> m_colorSprites;

    std::unordered_map<SelectArtType, std::vector<Ref<geode::Button>>> m_artNodes;
};

}