#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

class Group : public cocos2d::CCNode {};

class $module(PreviewObjectColors) {
    geode::Ref<cocos2d::CCNode> m_buttonContainer = nullptr;
    geode::Ref<geode::Button> m_buttonForScroll = nullptr;

    bool m_callbacksAdded = false;

    bool onToggled(bool state);
    void onEditor();
    void editColor();
    void setupButton(bool scrollable);

    GJSpriteColor* getBaseColor();
    GJSpriteColor* getDetailColor();

    void setButtonVisible(geode::Button* button);
};

class $modify(POCEditorUI, EditorUI) {
    $registerHooks(PreviewObjectColors, true)

    struct Fields {
        geode::Ref<GameObject> m_defaultObject = nullptr;
    };

    GameObject* createObject(int objectID, cocos2d::CCPoint position);

	static void _onModify(auto& self) {
        (void) self.setHookPriorityBeforePre("EditorUI::editObject", "ninkaz.editor_utils"); 
    }
};

class $modify(POCLevelEditorLayer, LevelEditorLayer) {
	void updateObjectColors(float dt);
};