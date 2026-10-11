#pragma once

#include "module/Module.hpp"
#include <Geode/modify/CCTextInputNode.hpp>

class $module(NegateInput) {
    std::set<geode::Ref<CCTextInputNode>> m_activeNodes{};
    
    bool onToggled(bool state);
    bool onSettingChanged(std::string_view key, const matjson::Value& value);
};

class $modify(NECCTextInputNode, CCTextInputNode) {
    $registerHooks(NegateInput)

    struct Fields {
        geode::Ref<geode::Button> m_button = nullptr;
    };

	bool allowedInput();
    bool init(float width, float height, char const* placeholder, char const* textFont, int fontSize, char const* labelFont);
    bool onTextFieldInsertText(cocos2d::CCTextFieldTTF* pSender, char const* text, int nLen, cocos2d::enumKeyCodes keyCodes);
    void onClickTrackNode(bool selected);
    void toggleOn();
    void toggleOff(bool unscheduleFocus);

	void onNegate();
    void showOnFocus(float dt);
};