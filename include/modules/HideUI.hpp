#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $module(HideUI) {
    Ref<CCNode> m_oldBEButton;
    Ref<CCMenuItemToggler> m_hideButton;
    bool m_addedCallbacks;
    
    bool onToggled(bool state);
    void onEditor();
};

class $modify(HUIEditorUI, EditorUI) {
    $registerHooks(HideUI)

    void updateButtons();
    void updateCreateMenu(bool selectTab);
    void clickOnPosition(cocos2d::CCPoint position);
    void toggleMode(CCObject* sender);
    void selectBuildTab(int tab);
};