#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $module(EditorSliderFix) {
    float m_lastObjectX = 0.f;

    void onEditor();
    bool onToggled(bool state);
};

class $modify(ESFEditorUI, EditorUI) {
    $registerHooks(EditorSliderFix)
    
    void sliderChanged(cocos2d::CCObject* sender);
	void updateSlider();
};