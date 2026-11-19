#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>
#include "nodes/LabelListener.hpp"

namespace tinker::ui {

class LayerInput : public cocos2d::CCNode {
public:
    static LayerInput* create();

    void setText(geode::ZStringView text, bool runCallback = false);
    void updateDisplay();
protected:
    bool init();

    geode::NineSlice* m_background = nullptr;
    geode::NineSlice* m_lockBorder = nullptr;
    geode::TextInput* m_input = nullptr;
    geode::Button* m_lockBtn = nullptr;
    geode::NineSlice* m_lockSprContainer = nullptr;

    bool m_lastLayerLockState = false;
};

}

class $module(ImprovedLayers) {
    bool onToggled(bool state);

    void onEditor();
    void cleanupLayerMenu();
    void revertLayerMenu();
    void updateDisplay();

    geode::Ref<tinker::ui::LabelListener> m_labelListener = nullptr;
    geode::Ref<tinker::ui::LayerInput> m_layerInput = nullptr;
    geode::Ref<CCMenuItemSpriteExtra> m_nextFreeBtn = nullptr;
    geode::Ref<cocos2d::CCLabelBMFont> m_oldLabel = nullptr;
    geode::Ref<cocos2d::CCNode> m_oldLayerLockButton = nullptr;
    cocos2d::CCNode* m_layerLockButtonParent = nullptr;
    std::unordered_map<cocos2d::CCNode*, float> m_oldScales{};
    std::unordered_map<cocos2d::CCNode*, cocos2d::CCPoint> m_oldPositions{};
    GLubyte m_oldLockOpacity = 0;
    cocos2d::CCSize m_oldMenuSize{};
};

class $modify(ILEditorUI, EditorUI) {
    $registerHooks(ImprovedLayers)

    void onLockLayer(cocos2d::CCObject* sender);
    void updateGroupIDLabel();
};