#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class Group : public CCNode {};

class $editorModule(PreviewObjectColors) {
    void onEditor() override;
    void onUpdateButtons();
};

class $modify(POCEditorUI, EditorUI) {
    $registerEditorHooks(PreviewObjectColors, true)

    struct Fields {
        Ref<GameObject> m_defaultObject;
    };

    void editObject(cocos2d::CCObject* sender);
    GameObject* createObject(int objectID, cocos2d::CCPoint position);
	void updateObjectColors(float dt);

	static void _onModify(auto& self) {
        (void) self.setHookPriorityBeforePre("EditorUI::editObject", "ninkaz.editor_utils"); 
    }
};