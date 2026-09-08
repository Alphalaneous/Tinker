#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>
#include "nodes/LabelListener.hpp"

namespace tinker::ui {

class LayerInput : public CCNode {
public:
    static LayerInput* create();

    void setText(ZStringView text, bool runCallback = false);
    void updateDisplay();
protected:
    bool init();

    geode::NineSlice* m_background;
    geode::NineSlice* m_lockBorder;
    geode::TextInput* m_input;
    geode::Button* m_lockBtn;
    geode::NineSlice* m_lockSprContainer;

    bool m_lastLayerLockState;
};

}

class $module(ImprovedLayers) {
    bool onToggled(bool state);

    void onEditor();
    void cleanupLayerMenu();
    void revertLayerMenu();
    void updateDisplay();

    Ref<tinker::ui::LabelListener> m_labelListener;
    Ref<tinker::ui::LayerInput> m_layerInput;
    Ref<CCMenuItemSpriteExtra> m_nextFreeBtn;
    Ref<CCLabelBMFont> m_oldLabel;
    Ref<CCNode> m_oldLayerLockButton;
    CCNode* m_layerLockButtonParent;
    Ref<geode::TextInput> m_oldBEInput;
    Ref<CCNode> m_oldBELayerLock;
    Ref<CCNode> m_oldBENextFree;
    std::unordered_map<CCNode*, float> m_oldScales;
    std::unordered_map<CCNode*, CCPoint> m_oldPositions;
    GLubyte m_oldLockOpacity;
    CCSize m_oldMenuSize;
};

class $modify(ILEditorUI, EditorUI) {
    $registerHooks(ImprovedLayers)

    void onLockLayer(CCObject* sender);
};