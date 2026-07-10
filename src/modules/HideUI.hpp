#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $editorModule(HideUI) {
    Ref<CCNode> m_oldBEButton;
    
    void onEditor();
};

class $modify(HUIEditorUI, EditorUI) {
    $registerEditorHooks(HideUI)

    void updateButtons();
    void updateCreateMenu(bool selectTab);
    void clickOnPosition(cocos2d::CCPoint position);
    void toggleMode(CCObject* sender);
    void selectBuildTab(int tab);
};