#pragma once

#include "module/Module.hpp"
#include <Geode/modify/LevelEditorLayer.hpp>

class $module(AlternateCleanStartPosLocation) {

    bool onToggled(bool state);
    void onEditor();
    void onStartPos();
    void onRemoveStartPos();
    UndoObject* undoObjectForStartPos(GameObject* object);

    Ref<CCMenuItemSpriteExtra> m_originalButton = nullptr;
    CCNode* m_container = nullptr;
    GameObject* m_lastStartPos = nullptr;
    GameObject* m_lastStartPos2 = nullptr;

    std::vector<Ref<GameObject>> m_startPositionsDuringPlaytest;
};

class $modify(ACSPLLevelEditorLayer, LevelEditorLayer) {
    $registerHooks(AlternateCleanStartPosLocation)

    void onStopPlaytest();
    void addSpecial(GameObject* object);
    void removeSpecial(GameObject* object);
};