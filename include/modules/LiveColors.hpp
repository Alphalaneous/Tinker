#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

namespace tinker::ui {
    class ColorVisualButton;
}

class $module(LiveColors) {
    CCMenu* m_colorsMenu;
    std::vector<tinker::ui::ColorVisualButton*> m_buttons;
    int m_lastBtnCount = 0;
    int m_availableBtnCount = 0;

    void onEditor();
    void showMenu(bool show);
};

class $modify(LCEditorUI, EditorUI) {
    $registerHooks(LiveColors, true)

	static void _onModify(auto& self) {
        (void) self.setHookPriorityAfterPost("EditorUI::init", "hjfod.betteredit");
    }

    struct Fields {
        bool m_uiVisible = true;
    };
    
    void showUI(bool show);
};

class $modify(LCLevelEditorLayer, LevelEditorLayer) {
	void checkColors(float dt);
};