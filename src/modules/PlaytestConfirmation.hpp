#pragma once

#include "../Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $editorModule(PlaytestConfirmation) {
    bool onToggled(bool state) override;
};

class $modify(PCEditorUI, EditorUI) {
    $registerEditorHooks(PlaytestConfirmation)

    struct Fields {
        bool m_doPlaytest;
    };

    void onPlaytest(CCObject* sender);
};