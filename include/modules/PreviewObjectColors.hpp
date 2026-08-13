#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

class Group : public CCNode {};

class $module(PreviewObjectColors) {
    void onEditor();
    void editColor();
    void setButtonVisible(geode::Button* button);
    void setButtonScale(geode::Button* button, float scale);
};

class $modify(POCEditorUI, EditorUI) {
    $registerHooks(PreviewObjectColors, true)

    struct Fields {
        Ref<GameObject> m_defaultObject;
    };

    GameObject* createObject(int objectID, cocos2d::CCPoint position);

	static void _onModify(auto& self) {
        (void) self.setHookPriorityBeforePre("EditorUI::editObject", "ninkaz.editor_utils"); 
    }
};

class $modify(POCLevelEditorLayer, LevelEditorLayer) {
	void updateObjectColors(float dt);
};