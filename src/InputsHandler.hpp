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
        std::set<FLAlertLayer*> m_activeAlerts;

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
    };

    bool init(LevelEditorLayer* editorLayer);
    void onScroll();
    void scrollWheel(float y, float x);
    void disableBetterEditHook();

    void checkScrolling(float dt);

    void addActiveAlert(FLAlertLayer* alert);
    void removeActiveAlert(FLAlertLayer* alert);

    bool hasActiveAlerts();

    bool isNaturalScrollEnabled();
    
    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void blockPause();
    void unblockPause();

    void onPause(cocos2d::CCObject* sender);

    static InputEditorUI* get();
};

class $modify(InputEditorPauseLayer, EditorPauseLayer) {
    void customSetup();
};