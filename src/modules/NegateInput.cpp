#include "NegateInput.hpp"

bool NegateInput::onSettingChanged(std::string_view key, const matjson::Value& value) {
    if (key == "always-show-negate-button") {
        return true;
    }
    return false;
}

bool NECCTextInputNode::allowedInput() {
    if (!LevelEditorLayer::get()) return false;
    for (char c : std::string_view(m_allowedChars)) {
        if (!((c >= '0' && c <= '9') || c == '-' || c == '.')) {
            return false;
        }
    }
    return true;
}

bool NECCTextInputNode::init(float width, float height, char const* placeholder, char const* textFont, int fontSize, char const* labelFont) {
    if (!CCTextInputNode::init(width, height, placeholder, textFont, fontSize, labelFont)) return false;

    runAction(CallFuncExt::create([this] {
        if (!allowedInput() || !NegateInput::getSetting<bool, "show-negate-button">()) return;

        auto fields = m_fields.self();

        auto spr = CCSprite::createWithSpriteFrameName("edit_delCBtn_001.png");

        auto btnSpr = ButtonSprite::create(spr, 30, 1, 30, 1, false, "GJ_button_06.png", false);
        btnSpr->setScale(0.3f);

        fields->m_button = geode::Button::createWithNode(btnSpr, [this] (auto sender) {
            onNegate();
        });

        fields->m_button->setID("negate-button"_spr);

        fields->m_button->setPosition(getContentSize() / 2 + CCPoint{1, -3});

        schedule(schedule_selector(NECCTextInputNode::showOnFocus));

        addChild(fields->m_button);
    }));

    return true;
}

void NECCTextInputNode::showOnFocus(float dt) {
    auto fields = m_fields.self();
    if (fields->m_button) {
        fields->m_button->setVisible(m_selected || NegateInput::getSetting<bool, "always-show-negate-button">());
    }
}

bool NECCTextInputNode::onTextFieldInsertText(CCTextFieldTTF* pSender, char const* text, int nLen, enumKeyCodes keyCodes) {
    if (allowedInput() && NegateInput::getSetting<bool, "enable-negate-keybind">()) {
        if (std::string_view(text) == "n") {
            onNegate();
            return true;
        }
    }
    return CCTextInputNode::onTextFieldInsertText(pSender, text, nLen, keyCodes);
}

void NECCTextInputNode::onNegate() {
    auto numRes = numFromString<double>(getString());
    if (numRes) {
        auto num = numRes.unwrap();
        num *= -1;
        setString(numToString(num));
    }
}
