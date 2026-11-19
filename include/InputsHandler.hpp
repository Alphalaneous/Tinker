#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/AppDelegate.hpp>
#include <Geode/loader/Priority.hpp>
#include "utils/Mod.hpp"

#ifdef GEODE_IS_MOBILE
class $modify(InputAppDelegate, AppDelegate) {
    static void onModify(auto& self) {
        for (const auto& [k, v] : self.m_hooks) { 
            v->setAutoEnable(false);
        }

        auto shouldLoad = tinker::utils::shouldLoadTinker();
        for (const auto& [k, v] : self.m_hooks) { 
            (void) v->toggle(shouldLoad);
        }
    }
    
    void applicationDidEnterBackground();
};
#endif

class $modify(InputEditorUI, EditorUI) {

    static void onModify(auto& self) {
        (void) self.setHookPriorityPre("EditorUI::scrollWheel", geode::Priority::EarlyPre - 1);

        for (const auto& [k, v] : self.m_hooks) { 
            v->setAutoEnable(false);
        }

        auto shouldLoad = tinker::utils::mod::shouldLoadTinker();
        for (const auto& [k, v] : self.m_hooks) { 
            (void) v->toggle(shouldLoad);
        }
    }

    struct Fields {
        std::set<cocos2d::CCNode*> m_activeAlerts{};
        std::set<geode::TextInput*> m_textInputs{};
        std::set<CCTextInputNode*> m_allTextInputs{};

        geode::Ref<cocos2d::CCActionInterval> m_moveX = nullptr;
        geode::Ref<cocos2d::CCActionInterval> m_moveY = nullptr;
        geode::Ref<cocos2d::CCActionInterval> m_scale = nullptr;

        cocos2d::CCPoint m_targetPos{};
        cocos2d::CCPoint m_startSwipe{};
        cocos2d::CCPoint m_scroll{};

        bool m_activeScroll = false;
        bool m_activeZoom = false;
        bool m_tabModifierHeld = false;
        bool m_blockPause = false;

        float m_scrollFromEditor = false;
        float m_targetScale = false;
        float m_speedScale = false;

        geode::Ref<cocos2d::CCTouch> m_touch1 = nullptr;
        geode::Ref<cocos2d::CCTouch> m_touch2 = nullptr;

        float m_initialDistance = 0.f;
        float m_initialScale = 1.f;
        cocos2d::CCPoint m_touchMidPoint{};

        bool m_isPinching = false;
        float m_lastAngle = 0.f;

        bool m_inGizmo = false;
        cocos2d::CCTargetedTouchDelegate* m_touchedGizmoChild = nullptr;
        GameObject* m_activeGizmoTouch = nullptr;

        //tinker::ui::TouchForward* m_forward = nullptr;
    };

    static bool isEnabled() {
        return true;
    }

    bool init(LevelEditorLayer* editorLayer);
    bool onScroll();
    bool scrollWheel(float y, float x);

    void addActiveAlert(CCNode* alert);
    void removeActiveAlert(CCNode* alert);

    void addActiveInput(CCTextInputNode* input);
    void removeActiveInput(CCTextInputNode* input);

    bool hasActiveAlerts();

    bool isNaturalScrollEnabled();

    bool tryCancelGizmo(cocos2d::CCTouch* touch);

    bool handleAction(int action, bool down, double timestamp);

    void blockPause();
    void unblockPause();
    cocos2d::CCPoint getRealMousePos();
    bool isInDevtools();

    CCMenuItemSpriteExtra* getEditButtonByTag(int tag);

    static void addTextInput(geode::TextInput* input);
    void cancelAllTextInputs();

    void onPause(cocos2d::CCObject* sender);

    static InputEditorUI* get();
};

class $modify(InputLevelEditorLayer, LevelEditorLayer) {
    void checkScrolling(float dt);
};

class $modify(InputEditorPauseLayer, EditorPauseLayer) {
    static void onModify(auto& self) {
        for (const auto& [k, v] : self.m_hooks) { 
            v->setAutoEnable(false);
        }

        auto shouldLoad = tinker::utils::mod::shouldLoadTinker();
        for (const auto& [k, v] : self.m_hooks) { 
            (void) v->toggle(shouldLoad);
        }
    }

    void customSetup();
};
