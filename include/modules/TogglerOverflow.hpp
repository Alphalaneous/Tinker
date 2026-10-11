#pragma once

#include "module/Module.hpp"

namespace tinker::ui {

class ToggleContainer : public cocos2d::CCLayer {
public:
    static ToggleContainer* create(EditorUI* editorUI);
    void updateScale(float scale);

    void show(bool show);
    void updateContainer(bool addBack = true);
    void showToggle(cocos2d::CCNode* node, bool show, bool animate);

protected:
    bool init(EditorUI* editorUI);

    void registerWithTouchDispatcher();
    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);

    std::vector<geode::Ref<cocos2d::CCNode>> m_nodes{};
    EditorUI* m_editorUI = nullptr;
    geode::CircleButtonSprite* m_expandOn = nullptr;
    geode::CircleButtonSprite* m_expandOff = nullptr;
    cocos2d::CCMenu* m_mainMenu = nullptr;
    geode::NineSlice* m_background = nullptr;
    cocos2d::CCSprite* m_separator = nullptr;
    geode::Button* m_expandButton = nullptr;

    bool m_expanded = false;
    bool m_isUsed = false;

    static constexpr float ScaleMult = 0.9f;
    static constexpr float Height = 90.f;
    static constexpr float LeftOffset = 21.f;
    static constexpr float RightOffset = 3.f;
};

}

class $module(TogglerOverflow) {
    geode::Ref<tinker::ui::ToggleContainer> m_container = nullptr;
    bool m_addedCallbacks = false;
    float m_scale = false;

    bool onToggled(bool state);
    void onEditor();
    void updateContainer();
};
