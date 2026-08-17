#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/AppDelegate.hpp>

using namespace geode::prelude;

namespace tinker::ui {

class TouchForward : public CCLayer {
public:
    using Next = geode::Function<bool(CCTouch*)>;

    struct TouchHook {
        int priority;
        geode::Function<bool()> enabled;
        geode::Function<bool(CCTouch*, geode::Function<bool(CCTouch*)>)> beganCallback;
        geode::Function<void(CCTouch*, geode::Function<void(CCTouch*)>)> movedCallback;
        geode::Function<void(CCTouch*, geode::Function<void(CCTouch*)>)> endedCallback;
        geode::Function<void(CCTouch*, geode::Function<void(CCTouch*)>)> cancelledCallback;
    };

    static TouchForward* create(EditorUI* editorUI);
    static TouchForward* get();

    template<class T>
    void registerTouch(int priority) {
        m_touchHooks.push_back(TouchHook{priority, 
            [] -> bool {
                return T::isEnabled();
            },
            [] (CCTouch* touch, geode::Function<bool(CCTouch*)> next) -> bool {
                return T::get()->onTouchBegan(touch, std::move(next));
            },
            [] (CCTouch* touch, geode::Function<void(CCTouch*)> next) {
                T::get()->onTouchMoved(touch, std::move(next));
            },
            [] (CCTouch* touch, geode::Function<void(CCTouch*)> next) {
                T::get()->onTouchEnded(touch, std::move(next));
            },
            [] (CCTouch* touch, geode::Function<void(CCTouch*)> next) {
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
    decltype(auto) dispatch(size_t i, CCTouch* touch, CallbackGetter&& getCallback, FinalCall&& finalCall) {
        std::vector<TouchHook*> enabledHooks;
        
        for (auto& hook : m_touchHooks) {
            if (hook.enabled()) {
                enabledHooks.push_back(&hook);
            }
        }
        
        if (i == enabledHooks.size()) {
            return finalCall(touch);
        }

        return getCallback(*enabledHooks[i]) (touch,
            [this, i, &getCallback, &finalCall] (CCTouch* touch) -> decltype(auto) {
                return dispatch(i + 1, touch, getCallback, finalCall);
            }
        );
    }

    void registerWithTouchDispatcher() override;

    bool ccTouchBegan(CCTouch* touch, CCEvent* event) override;
    void ccTouchMoved(CCTouch* touch, CCEvent* event) override;
    void ccTouchEnded(CCTouch* touch, CCEvent* event) override;
    void ccTouchCancelled(CCTouch* touch, CCEvent* event) override;

    void cancelAllTouches();
protected:

    bool init(EditorUI* editorUI);

    bool m_skipErase;
    std::vector<TouchHook> m_touchHooks;
    std::unordered_set<Ref<CCTouch>> m_touches;
    EditorUI* m_editorUI;
};
}

#ifdef GEODE_IS_MOBILE
class $modify(InputAppDelegate, AppDelegate) {
    void applicationDidEnterBackground();
};
#endif

class $modify(InputEditorUI, EditorUI) {

    static void onModify(auto& self) {
        (void) self.setHookPriorityPre("EditorUI::scrollWheel", Priority::EarlyPre - 1);
    }

    struct Fields {
        std::set<CCNode*> m_activeAlerts;
        std::set<TextInput*> m_textInputs;
        std::set<CCTextInputNode*> m_allTextInputs;

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

        bool m_blockPinching = false;
        bool m_isPinching = false;
        float m_lastAngle;

        tinker::ui::TouchForward* m_forward;
    };

    static bool isEnabled() {
        return true;
    }

    bool init(LevelEditorLayer* editorLayer);
    void onScroll();
    void scrollWheel(float y, float x);
    void disableBetterEditHook();


    void addActiveAlert(CCNode* alert);
    void removeActiveAlert(CCNode* alert);

    void addActiveInput(CCTextInputNode* input);
    void removeActiveInput(CCTextInputNode* input);

    bool hasActiveAlerts();

    bool isNaturalScrollEnabled();

    CCPoint getTouchLocation(CCTouch* touch);

    bool onTouchBegan(CCTouch* touch, geode::Function<bool(CCTouch* touch)> next);
    void onTouchMoved(CCTouch* touch, geode::Function<void(CCTouch* touch)> next);
    void onTouchEnded(CCTouch* touch, geode::Function<void(CCTouch* touch)> next);
    void onTouchCancelled(CCTouch* touch, geode::Function<void(CCTouch* touch)> next);
    
    void blockPause();
    void unblockPause();
    CCPoint getRealMousePos();

    void blockPinch(bool block);

    CCMenuItemSpriteExtra* getEditButtonByTag(int tag);

    static void addTextInput(TextInput* input);
    void cancelAllTextInputs();

    void onPause(cocos2d::CCObject* sender);

    static InputEditorUI* get();
};

class $modify(InputLevelEditorLayer, LevelEditorLayer) {
    void checkScrolling(float dt);
};

class $modify(InputEditorPauseLayer, EditorPauseLayer) {
    void customSetup();
};
