#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

class $modify(InputEditorUI, EditorUI) {

    static void onModify(auto& self) {
        (void) self.setHookPriorityPre("EditorUI::scrollWheel", Priority::EarlyPre - 1);
    }

    struct Fields {
        float m_targetScale;
        CCPoint m_targetPos;
        std::set<FLAlertLayer*> m_activeAlerts;
        bool m_activeScroll;
        Ref<CCActionInterval> m_moveX = nullptr;
        Ref<CCActionInterval> m_moveY = nullptr;
        bool m_activeZoom;
        Ref<CCActionInterval> m_scale = nullptr;
        CCPoint m_startSwipe;
        bool m_tabModifierHeld;
    };

    bool init(LevelEditorLayer* editorLayer);
    void onScroll(float x, float y);
    void scrollWheel(float y, float x);
    void disableBetterEditHook();

    void checkScrolling(float dt);

    void addActiveAlert(FLAlertLayer* alert);
    void removeActiveAlert(FLAlertLayer* alert);

    bool isNaturalScrollEnabled();
    
    static InputEditorUI* get();
};