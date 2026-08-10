#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $module(SingleDeselect) {
	void onEditor();
    bool onToggled(bool state);
    bool onSettingChanged(std::string_view key, const matjson::Value& value);
	
	bool m_keyHeld;
	bool m_blockDeselect;
};

class $modify(SDEditorUI, EditorUI) {
	$registerHooks(SingleDeselect)

	GameObject* selectedObjectAtPosition(CCPoint pos);
	void selectObject(GameObject* object, bool ignoreFilter);
    void selectObjects(CCArray* objects, bool ignoreFilter);
	void createUndoSelectObject(bool redo);
	void ccTouchEnded(CCTouch* touch, CCEvent* event);
	bool getKeyPressed();

    CCArray* pasteObjects(gd::string str, bool withColor, bool noUndo);
};

