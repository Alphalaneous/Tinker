#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $module(PlaytestConfirmation) {
    bool onToggled(bool state);
};

class $modify(PCEditorUI, EditorUI) {
    $registerHooks(PlaytestConfirmation)

    struct Fields {
        bool m_doPlaytest;
    };

    void onPlaytest(CCObject* sender);
};