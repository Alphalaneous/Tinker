#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>

using namespace geode::prelude;

class $modify(InputEditorUI, EditorUI) {

    static void onModify(auto& self) {
        (void) self.setHookPriorityPre("EditorUI::scrollWheel", Priority::EarlyPre - 1);
    }

    struct Fields {
        std::set<CCNode*> m_activeAlerts;
        std::set<TextInput*> m_textInputs;

        Ref<CCActionInterval> m_moveX = nullptr;
        Ref<CCActionInterval> m_moveY = nullptr;
        Ref<CCActionInterval> m_scale = nullptr;

        CCPoint m_targetPos;
        CCPoint m_startSwipe;
        CCPoint m_scroll;

        bool m_activeScroll;
        bool m_activeZoom;
        bool m_tabModifierHeld;
        bool m_blockPause;

        float m_targetScale;

        float m_speedScale;

        std::set<Ref<CCTouch>> m_touches;
        float m_initialDistance = 0.f;
        float m_initialScale = 1.f;
        CCPoint m_touchMidPoint;
    };

    bool init(LevelEditorLayer* editorLayer);
    void onScroll();
    void scrollWheel(float y, float x);
    void disableBetterEditHook();

    void checkScrolling(float dt);

    void addActiveAlert(CCNode* alert);
    void removeActiveAlert(CCNode* alert);

    bool hasActiveAlerts();

    bool isNaturalScrollEnabled();
    
    bool ccTouchBegan(CCTouch* touch, CCEvent* event);
    void ccTouchMoved(CCTouch* touch, CCEvent* event);
    void ccTouchEnded(CCTouch* touch, CCEvent* event);
    void ccTouchCancelled(CCTouch* touch, CCEvent* event);

    void blockPause();
    void unblockPause();
    CCPoint getRealMousePos();

    void addTextInput(TextInput* input);
    void removeTextInput(TextInput* input);

    void onPause(cocos2d::CCObject* sender);

    static InputEditorUI* get();
};

class $modify(InputEditorPauseLayer, EditorPauseLayer) {
    void customSetup();
};