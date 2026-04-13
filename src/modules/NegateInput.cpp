#include "NegateInput.hpp"

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
        auto menu = CCMenu::create();
        menu->setAnchorPoint({0.f, 0.5f});
        menu->ignoreAnchorPointForPosition(false);
        menu->setPosition((getContentSize()/2) - 4);
        menu->setContentSize({12, 12});
        menu->setID("negate-menu"_spr);

        auto spr = CCSprite::createWithSpriteFrameName("edit_delCBtn_001.png");

        auto btnSpr = ButtonSprite::create(spr, 30, 1, 30, 1, false, "GJ_button_06.png", false);
        btnSpr->setScale(0.3f);

        auto btn = CCMenuItemExt::createSpriteExtra(btnSpr, [this] (auto sender) {
            onNegate();
        });

        btn->setPosition(menu->getContentSize()/2);
        btn->setID("negate-button"_spr);

        menu->addChild(btn);

        addChild(menu);
    }));

    return true;
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
