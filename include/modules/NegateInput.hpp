#pragma once

#include "module/Module.hpp"
#include <Geode/modify/CCTextInputNode.hpp>

class $module(NegateInput) {
    std::set<Ref<CCTextInputNode>> m_activeNodes;
    
    bool onToggled(bool state);
    bool onSettingChanged(std::string_view key, const matjson::Value& value);
};

class $modify(NECCTextInputNode, CCTextInputNode) {
    $registerHooks(NegateInput)

    struct Fields {
        Ref<geode::Button> m_button;
    };

	bool allowedInput();
    bool init(float width, float height, char const* placeholder, char const* textFont, int fontSize, char const* labelFont);
    bool onTextFieldInsertText(CCTextFieldTTF* pSender, char const* text, int nLen, enumKeyCodes keyCodes);
    void onClickTrackNode(bool selected);
    void toggleOn();
    void toggleOff(bool unscheduleFocus);

	void onNegate();
    void showOnFocus(float dt);
};