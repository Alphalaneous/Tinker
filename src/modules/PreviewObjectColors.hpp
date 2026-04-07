#pragma once

#include "../Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class Group : public CCNode {};

class $editorModule(PreviewObjectColors) {
    struct ColorData {
		ccColor3B color;
		bool blending;
		GLubyte opacity;
	};

    Ref<GameObject> m_defaultObject;

    void onEditor() override;
    void onSave() override;
    void onUpdateButtons() override;
};

class $modify(POCEditorUI, EditorUI) {
    $registerEditorHooks(PreviewObjectColors, true)

    void editObject(cocos2d::CCObject* sender);
    GameObject* createObject(int p0, cocos2d::CCPoint p1);
	PreviewObjectColors::ColorData getActiveColor(int colorID);
	bool isColorable(GameObject* object);
	void updateButton(CCNode* btn);
	void updateObjectColors(float dt);

	static void _onModify(auto& self) {
        (void) self.setHookPriorityBeforePre("EditorUI::editObject", "ninkaz.editor_utils"); 
    }
};