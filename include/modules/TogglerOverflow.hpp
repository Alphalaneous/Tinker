#pragma once

#include "module/Module.hpp"

namespace tinker::ui {

class ToggleContainer : public CCLayer {
public:
    static ToggleContainer* create(EditorUI* editorUI);
    void updateScale(float scale);

    void show(bool show);
    void updateContainer(bool addBack = true);
    void showToggle(CCNode* node, bool show, bool animate);

protected:
    bool init(EditorUI* editorUI);

    void registerWithTouchDispatcher();
    bool ccTouchBegan(CCTouch* touch, CCEvent* event);

    std::vector<Ref<CCNode>> m_nodes;
    EditorUI* m_editorUI;
    CircleButtonSprite* m_expandOn;
    CircleButtonSprite* m_expandOff;
    CCMenu* m_mainMenu;
    NineSlice* m_background;
    CCSprite* m_separator;
    Button* m_expandButton;

    bool m_expanded;
    bool m_isUsed;

    static constexpr float ScaleMult = 0.91f;
    static constexpr float Height = 90.f;
    static constexpr float LeftOffset = 21.f;
    static constexpr float RightOffset = 3.f;
};

}

class $module(TogglerOverflow) {
    Ref<tinker::ui::ToggleContainer> m_container;
    bool m_addedCallbacks;

    bool onToggled(bool state);
    void onEditor();
    void updateContainer();
};
