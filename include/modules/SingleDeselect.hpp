#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $module(SingleDeselect) {
	void onEditor();
    bool onToggled(bool state);
    bool onSettingChanged(std::string_view key, const matjson::Value& value);
	
	bool m_keyHeld = false;
	bool m_blockDeselect = false;
};

class $modify(SDEditorUI, EditorUI) {
	$registerHooks(SingleDeselect)

	GameObject* selectedObjectAtPosition(cocos2d::CCPoint pos);
	void selectObject(GameObject* object, bool ignoreFilter);
    void selectObjects(cocos2d::CCArray* objects, bool ignoreFilter);
	void createUndoSelectObject(bool redo);
	void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
	bool getKeyPressed();

    cocos2d::CCArray* pasteObjects(gd::string str, bool withColor, bool noUndo);
};

