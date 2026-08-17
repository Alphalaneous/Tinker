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

    bool onToggled(bool state);
    void onEditor();
    void showMenu(bool show);
    void updateUI(float scale);
};

class $modify(LCLevelEditorLayer, LevelEditorLayer) {
	void checkColors(float dt);
};