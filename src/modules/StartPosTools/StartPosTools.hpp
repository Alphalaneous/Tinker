#pragma once

#include "../../Module.hpp"
#include "StartPosOverlay.hpp"
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

class $globalModule(StartPosTools) {
    bool onSettingChanged(std::string_view key, const matjson::Value& value) override;
};

class $modify(SPTEditorUI, EditorUI) {
    $registerGlobalHooks(StartPosTools)

    struct Fields {
        Ref<tinker::ui::StartPosOverlay> m_overlay;
        CCMenuItemSpriteExtra* m_startPosBtn;
        bool m_fromStart;
        bool m_currentlyPlaying = false;
        geode::NineSlice* m_switcherContainer;
        CCLabelBMFont* m_switcherLabel;
        geode::Button* m_prevButton;
        geode::Button* m_nextButton;
    };

    bool init(LevelEditorLayer* editorLayer);
    void updateOverlay();
    void showSwitcher();
    void hideSwitcher(float dt);
    void showUI(bool show);
    void onPlaytest(cocos2d::CCObject* sender);
    void updatePlaytestMenu();
    void deselectAll();
    void deselectObject(GameObject* object);
    void selectObject(GameObject* object, bool ignoreFilter);
    void selectObjects(cocos2d::CCArray* objects, bool ignoreFilter);
    void onDeleteStartPos(cocos2d::CCObject* sender);
    void updateSwitcherLabel();
};

class $modify(SPTGJBaseGameLayer, GJBaseGameLayer) {
    $registerGlobalHooks(StartPosTools)

    void orderSpawnObjects();
};

class $modify(SPTLevelEditorLayer, LevelEditorLayer) {
    $registerGlobalHooks(StartPosTools)

    struct Fields {
        std::vector<Ref<StartPosObject>> m_startPositions;
        int m_startPosIndex = -1;
        int m_startPosIndexReal = -1;

        bool m_fromStart = false;
        Ref<StartPosObject> m_activeStartPos;
    };

    void addSpecial(GameObject* object);
    void removeSpecial(GameObject* object);
    void addStartPos(StartPosObject* startPos);
    void removeStartPos(StartPosObject* startPos);

    int getLastEnabledStartposIndex();

    unsigned int indexForStartPos(StartPosObject* startPos);
    void sortStartPositions();

    void prevStartPos();
    void nextStartPos();
    void setHasSwitched();
    bool hasSwitched();

    void restartFromStartPos();
    unsigned int getStartPosCount();
    unsigned int getActiveStartPosIndex();

    void startSwitcher(bool start);
    void setActiveStartPos(StartPosObject* startPos);
    void setStartPosIndex(int idx);
    StartPosObject* getActiveStartPos();
};