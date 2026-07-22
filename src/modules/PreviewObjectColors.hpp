#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>
#include <Geode/ui/Button.hpp>

class Group : public CCNode {};

class $editorModule(PreviewObjectColors) {
    void onEditor() override;
    void editColor();
    void setButtonVisible(geode::Button* button);
    void setButtonScale(geode::Button* button, float scale);
};

class $modify(POCEditorUI, EditorUI) {
    $registerEditorHooks(PreviewObjectColors, true)

    struct Fields {
        Ref<GameObject> m_defaultObject;
    };

    GameObject* createObject(int objectID, cocos2d::CCPoint position);
	void updateObjectColors(float dt);

	static void _onModify(auto& self) {
        (void) self.setHookPriorityBeforePre("EditorUI::editObject", "ninkaz.editor_utils"); 
    }
};