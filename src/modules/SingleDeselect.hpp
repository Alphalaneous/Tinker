#pragma once

#include "../Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $editorModule(SingleDeselect) {
	void onEditor() override;
    bool onToggled(bool state) override;
    bool onSettingChanged(std::string_view key, const matjson::Value& value) override;
	
	bool m_keyHeld;
	bool m_blockDeselect;
};

class $modify(SDEditorUI, EditorUI) {
	$registerEditorHooks(SingleDeselect)

	void deselectSpecificObject(CCPoint pos);
	void selectObject(GameObject* object, bool ignoreFilter);
    void selectObjects(CCArray* objects, bool ignoreFilter);
    void ccTouchEnded(CCTouch* touch, CCEvent* event);
	bool getKeyPressed();

    CCArray* pasteObjects(gd::string str, bool withColor, bool noUndo);
};

