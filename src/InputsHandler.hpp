#pragma once

#include "Geode/cocos/touch_dispatcher/CCTouch.h"
#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/AppDelegate.hpp>
#include <unordered_set>

using namespace geode::prelude;

class TouchForward : public CCLayer {
public:
    static TouchForward* create(EditorUI* editorUI);
    static TouchForward* get();

    void registerWithTouchDispatcher() override;

    bool ccTouchBegan(CCTouch* touch, CCEvent* event) override;
    void ccTouchMoved(CCTouch* touch, CCEvent* event) override;
    void ccTouchEnded(CCTouch* touch, CCEvent* event) override;
    void ccTouchCancelled(CCTouch* touch, CCEvent* event) override;

    void cancelAllTouches();
protected:

    bool init(EditorUI* editorUI);

    std::unordered_set<Ref<CCTouch>> m_touches;
    EditorUI* m_editorUI;
};

class $modify(InputAppDelegate, AppDelegate) {
    void applicationDidEnterBackground();
};

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

        Ref<CCTouch> m_touch1;
        Ref<CCTouch> m_touch2;

        float m_initialDistance = 0.f;
        float m_initialScale = 1.f;
        CCPoint m_touchMidPoint;

        bool m_isPinching = false;
        float m_lastAngle;

        TouchForward* m_forward;
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

    CCPoint getTouchLocation(CCTouch* touch);
    float getToolbarHeight();

    bool onTouchBegan(CCTouch* touch, geode::Function<bool(CCTouch* touch)> next);
    void onTouchMoved(CCTouch* touch, geode::Function<void(CCTouch* touch)> next);
    void onTouchEnded(CCTouch* touch, geode::Function<void(CCTouch* touch)> next);
    void onTouchCancelled(CCTouch* touch, geode::Function<void(CCTouch* touch)> next);
    
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