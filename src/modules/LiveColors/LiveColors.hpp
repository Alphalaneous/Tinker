#pragma once

#include "../../Module.hpp"
#include <Geode/modify/EditorUI.hpp>

namespace tinker::ui {
    class ColorVisualButton;
}

class $editorModule(LiveColors) {
    CCMenu* m_colorsMenu;
    std::vector<tinker::ui::ColorVisualButton*> m_buttons;
    int m_lastBtnCount = 0;
    int m_availableBtnCount = 0;

    void onEditor() override;
    void showMenu(bool show);

    void updateScale(float scale);
};

class $modify(LCEditorUI, EditorUI) {
    $registerEditorHooks(LiveColors, true)

	static void _onModify(auto& self) {
        (void) self.setHookPriorityAfterPost("EditorUI::init", "hjfod.betteredit");
    }

    struct Fields {
        bool m_uiVisible = true;
    };
    
    void showUI(bool show);
	void checkColors(float dt);
};