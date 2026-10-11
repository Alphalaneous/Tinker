#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/AppDelegate.hpp>
#include <Geode/loader/Priority.hpp>
#include "utils/Mod.hpp"

namespace tinker::ui {

class TouchForward : public cocos2d::CCLayer {
public:
    using Next = geode::Function<bool(cocos2d::CCTouch*)>;

    struct TouchHook {
        int priority = 0;
        geode::Function<bool()> enabled = nullptr;
        geode::Function<bool(cocos2d::CCTouch*, geode::Function<bool(cocos2d::CCTouch*)>)> beganCallback = nullptr;
        geode::Function<void(cocos2d::CCTouch*, geode::Function<void(cocos2d::CCTouch*)>)> movedCallback = nullptr;
        geode::Function<void(cocos2d::CCTouch*, geode::Function<void(cocos2d::CCTouch*)>)> endedCallback = nullptr;
        geode::Function<void(cocos2d::CCTouch*, geode::Function<void(cocos2d::CCTouch*)>)> cancelledCallback = nullptr;
    };

    static TouchForward* create(EditorUI* editorUI);
    static TouchForward* get();

    template<class T>
    void registerTouch(int priority) {
        m_touchHooks.push_back(TouchHook{priority, 
            [] -> bool {
                return T::isEnabled();
            },
            [] (cocos2d::CCTouch* touch, geode::Function<bool(cocos2d::CCTouch*)> next) -> bool {
                return T::get()->onTouchBegan(touch, std::move(next));
            },
            [] (cocos2d::CCTouch* touch, geode::Function<void(cocos2d::CCTouch*)> next) {
                T::get()->onTouchMoved(touch, std::move(next));
            },
            [] (cocos2d::CCTouch* touch, geode::Function<void(cocos2d::CCTouch*)> next) {
                T::get()->onTouchEnded(touch, std::move(next));
            },
            [] (cocos2d::CCTouch* touch, geode::Function<void(cocos2d::CCTouch*)> next) {
                T::get()->onTouchCancelled(touch, std::move(next));
            },
        });

        std::sort(m_touchHooks.begin(), m_touchHooks.end(), 
            [] (const auto& a, const auto& b) {
                return a.priority > b.priority;
            }
        );
    }

    template<class CallbackGetter, class FinalCall>
    decltype(auto) dispatch(size_t i, cocos2d::CCTouch* touch, CallbackGetter&& getCallback, FinalCall&& finalCall) {
        std::vector<TouchHook*> enabledHooks{};
        
        for (auto& hook : m_touchHooks) {
            if (hook.enabled()) {
                enabledHooks.push_back(&hook);
            }
        }
        
        if (i == enabledHooks.size()) {
            return finalCall(touch);
        }

        return getCallback(*enabledHooks[i]) (touch,
            [this, i, &getCallback, &finalCall] (cocos2d::CCTouch* touch) -> decltype(auto) {
                return dispatch(i + 1, touch, getCallback, finalCall);
            }
        );
    }

    void registerWithTouchDispatcher() override;

    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;

    void cancelAllTouches();
protected:

    bool init(EditorUI* editorUI);

    bool m_skipErase = false;
    std::vector<TouchHook> m_touchHooks{};
    std::unordered_set<geode::Ref<cocos2d::CCTouch>> m_touches{};
    EditorUI* m_editorUI = nullptr;
};
}

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

        bool m_blockPinching = false;
        bool m_isPinching = false;
        float m_lastAngle = 0.f;

        bool m_inGizmo = false;
        cocos2d::CCTargetedTouchDelegate* m_touchedGizmoChild = nullptr;
        GameObject* m_activeGizmoTouch = nullptr;

        tinker::ui::TouchForward* m_forward = nullptr;
    };

    static bool isEnabled() {
        return true;
    }

    bool init(LevelEditorLayer* editorLayer);
    void onScroll();
    void scrollWheel(float y, float x);

    void addActiveAlert(CCNode* alert);
    void removeActiveAlert(CCNode* alert);

    void addActiveInput(CCTextInputNode* input);
    void removeActiveInput(CCTextInputNode* input);

    bool hasActiveAlerts();

    bool isNaturalScrollEnabled();

    static cocos2d::CCPoint getTouchLocation(cocos2d::CCTouch* touch);

    bool tryCancelGizmo(cocos2d::CCTouch* touch);

    bool onTouchBegan(cocos2d::CCTouch* touch, geode::Function<bool(cocos2d::CCTouch* touch)> next);
    void onTouchMoved(cocos2d::CCTouch* touch, geode::Function<void(cocos2d::CCTouch* touch)> next);
    void onTouchEnded(cocos2d::CCTouch* touch, geode::Function<void(cocos2d::CCTouch* touch)> next);
    void onTouchCancelled(cocos2d::CCTouch* touch, geode::Function<void(cocos2d::CCTouch* touch)> next);
    
    void blockPause();
    void unblockPause();
    cocos2d::CCPoint getRealMousePos();
    bool isInDevtools();

    void blockPinch(bool block);

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
