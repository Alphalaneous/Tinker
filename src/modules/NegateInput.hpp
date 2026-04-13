#pragma once

#include "../Module.hpp"
#include <Geode/modify/CCTextInputNode.hpp>

class $editorModule(NegateInput) {};

class $modify(NECCTextInputNode, CCTextInputNode) {
    $registerEditorHooks(NegateInput)

	bool allowedInput();
    bool init(float width, float height, char const* placeholder, char const* textFont, int fontSize, char const* labelFont);
    bool onTextFieldInsertText(CCTextFieldTTF* pSender, char const* text, int nLen, enumKeyCodes keyCodes);
	void onNegate();
};