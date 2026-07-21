#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $editorModule(EditorSliderFix) {
    float m_lastObjectX = 0.f;

    void onEditor() override;
    bool onToggled(bool state) override;
};

class $modify(ESFEditorUI, EditorUI) {
    $registerEditorHooks(EditorSliderFix)
    
    void sliderChanged(cocos2d::CCObject* sender);
	void updateSlider();
};