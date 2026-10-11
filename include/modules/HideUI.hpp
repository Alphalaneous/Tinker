#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $module(HideUI) {
    geode::Ref<CCMenuItemToggler> m_hideButton = nullptr;
    bool m_addedCallbacks = false;
    
    bool onToggled(bool state);
    void onEditor();
};

class $modify(HUIEditorUI, EditorUI) {
    $registerHooks(HideUI)

    void updateButtons();
    void updateCreateMenu(bool selectTab);
    void clickOnPosition(cocos2d::CCPoint position);
    void toggleMode(cocos2d::CCObject* sender);
    void selectBuildTab(int tab);
};