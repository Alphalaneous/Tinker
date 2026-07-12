#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/SetGroupIDLayer.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/CCTouchDispatcher.hpp>
#include <Geode/modify/ButtonSprite.hpp>
#include "module/ModuleBase.hpp"

using namespace geode::prelude;

class $modify(MainLevelEditorLayer, LevelEditorLayer) {

    struct Fields {
        std::vector<std::shared_ptr<EditorModuleBase>> m_modules;
    };

    bool init(GJGameLevel* level, bool noUI);
    std::vector<std::shared_ptr<EditorModuleBase>>* getModules();
    void forEachModule(geode::Function<void(EditorModuleBase*)> moduleCallback);

    static MainLevelEditorLayer* get();
};

class $modify(MainEditorUI, EditorUI) {

    static EditorUI* s_editorUI;

    struct Fields {
        int m_lastObjectCount;
        bool m_wasPlatformer;
        bool m_uiVisible = true;
        KeyboardModifier m_lastModifier;

        ~Fields();
    };

    static void onModify(auto& self) {
        (void) self.setHookPriority("EditorUI::init", Priority::Late);
        (void) self.setHookPriorityPre("EditorUI::scrollWheel", Priority::EarlyPre - 1);
        (void) self.setHookPriority("EditorUI::updateCreateMenu", Priority::Replace);
    }

    bool init(LevelEditorLayer* editorLayer);
    void showUI(bool show);
    void mainUpdate(float dt);
    void fixTabPositions();
    void checkPlatformerState(MainEditorUI::Fields* fields);
    void checkObjectPlacement(MainEditorUI::Fields* fields);
    void checkModifierState(MainEditorUI::Fields* fields);
    void updateButtons();
    void deactivateScaleControl();
    void updateCreateMenu(bool selectTab);
    void updateZoom(float zoom);

    bool isUIVisible();
    static MainEditorUI* get();
};

class $modify(MainSetGroupIDLayer, SetGroupIDLayer) {

    bool init(GameObject* obj, cocos2d::CCArray* objs);
};

class $modify(MainEditorPauseLayer, EditorPauseLayer) {

    static EditorPauseLayer* s_editorPauseLayer;

    struct Fields {
        bool m_wasIgnored = false;
        ~Fields();
    };

    static EditorPauseLayer* get();
    bool init(LevelEditorLayer* layer);
    void saveLevel();
};