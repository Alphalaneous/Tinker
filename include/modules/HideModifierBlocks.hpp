#pragma once

#include "module/Module.hpp"
#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <unordered_map>
#include <unordered_set>

class $module(HideModifierBlocks) {
    std::unordered_set<GameObject*> m_hiddenObjects;

    bool onToggled(bool state);
};

class $modify(HMBGameObject, GameObject) {
    $registerHooks(HideModifierBlocks)

    void setVisible(bool visible);
    void setOpacity(unsigned char opacity);
};

class $modify(HMBLevelEditorLayer, LevelEditorLayer) {
    $registerHooks(HideModifierBlocks)

    struct Fields {
        std::unordered_map<GameObject*, bool> m_prevInvisible;
        std::unordered_map<GameObject*, bool> m_prevVisible;
    };

    void hideMatchingObjects();
    void restoreSavedObjects();

    void onPlaytest();
    void onStopPlaytest();

#if !defined(GEODE_IS_WINDOWS) && !defined(GEODE_IS_MACOS) && !defined(GEODE_IS_IOS)
    void onPausePlaytest();
    void onResumePlaytest();
#endif
};
