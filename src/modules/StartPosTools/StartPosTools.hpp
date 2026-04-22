#pragma once

#include "../../Module.hpp"
#include "StartPosOverlay.hpp"
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

class $editorModule(StartPosTools) {
    tinker::ui::StartPosOverlay* m_overlay;
    void onEditor() override;
    void updateOverlay();
};

class $modify(SPTEditorUI, EditorUI) {
    $registerEditorHooks(StartPosTools)

    struct Fields {
        CCMenuItemSpriteExtra* m_startPosBtn;
        bool m_fromStart;
    };

    void showUI(bool show);
    void onPlaytest(cocos2d::CCObject* sender);
    void updatePlaytestMenu();
    void deselectAll();
    void deselectObject(GameObject* object);
    void selectObject(GameObject* object, bool ignoreFilter);
    void selectObjects(cocos2d::CCArray* objects, bool ignoreFilter);
    bool shouldDeleteObject(GameObject* object);
    void onStartPosPlaytest(CCObject* sender);
};

class $modify(SPTGJBaseGameLayer, GJBaseGameLayer) {
    $registerEditorHooks(StartPosTools)

    void orderSpawnObjects();
};