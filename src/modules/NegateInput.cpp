#include "modules/NegateInput.hpp"

bool NegateInput::onToggled(bool state) {
    for (auto& node : m_activeNodes) {
        auto textInput = static_cast<NECCTextInputNode*>(node.data());
        if (state) {
            textInput->toggleOn();
        }
        else {
            textInput->toggleOff(true);
        }
    }
    return true;
}

bool NegateInput::onSettingChanged(std::string_view key, const matjson::Value& value) {
    auto state = value.asBool().unwrapOrDefault();

    if (key == "show-negate-button") {
        for (auto& node : m_activeNodes) {
            auto textInput = static_cast<NECCTextInputNode*>(node.data());
            if (state) {
                textInput->toggleOn();
            }
            else {
                textInput->toggleOff(false);
            }
        }
    }
    return true;
}

bool NECCTextInputNode::allowedInput() {
    if (!LevelEditorLayer::get()) return false;

    bool hadMinus = false;
    for (const char& c : std::string_view(m_allowedChars)) {
        if (c == '-') {
            hadMinus = true;
        }
        if (!(std::isdigit(static_cast<unsigned char>(c)) || c == '-' || c == '.')) {
            return false;
        }
    }
    if (!hadMinus) return false;
    return true;
}

bool NECCTextInputNode::init(float width, float height, char const* placeholder, char const* textFont, int fontSize, char const* labelFont) {
    if (!CCTextInputNode::init(width, height, placeholder, textFont, fontSize, labelFont)) return false;

    runAction(CallFuncExt::create([this] {
        if (!NegateInput::getSetting<bool, "show-negate-button">()) return;
        toggleOn();
    }));

    addOnEnterCallback([this] {
        if (NegateInput::get()) {
            NegateInput::get()->m_activeNodes.insert(this);
        }
    });
    addOnExitCallback([this] {
        if (NegateInput::get()) {
            NegateInput::get()->m_activeNodes.erase(this);
        }
    });

    return true;
}

void NECCTextInputNode::showOnFocus(float dt) {
    auto fields = m_fields.self();
    if (fields->m_button) {
        fields->m_button->setVisible(m_selected || NegateInput::getSetting<bool, "always-show-negate-button">());
    }
}

void NECCTextInputNode::toggleOn() {
    if (!allowedInput()) return;

    auto fields = m_fields.self();

    auto spr = CCSprite::createWithSpriteFrameName("edit_delCBtn_001.png");

    auto btnSpr = ButtonSprite::create(spr, 30, 1, 30.f, 1.f, false, "GJ_button_06.png", false);
    btnSpr->setScale(0.3f);

    fields->m_button = geode::Button::createWithNode(btnSpr, [this] (auto sender) {
        onNegate();
    });

    fields->m_button->setID("negate-button"_spr);
    fields->m_button->setPosition(getContentSize() / 2.f + CCPoint{1.f, -3.f});
    fields->m_button->setVisible(false);

    schedule(schedule_selector(NECCTextInputNode::showOnFocus));

    addChild(fields->m_button);
}

void NECCTextInputNode::toggleOff(bool unscheduleFocus) {
    auto fields = m_fields.self();

    if (fields->m_button) {
        fields->m_button->removeFromParent();
        fields->m_button = nullptr;
    }

    if (unscheduleFocus) {
        unschedule(schedule_selector(NECCTextInputNode::showOnFocus));
    }
}

void NECCTextInputNode::onClickTrackNode(bool selected) {
    auto fields = m_fields.self();

    if (NegateInput::getSetting<bool, "show-negate-button">()) {
        if (!fields->m_button) {
            toggleOn();
        }
    }
    else {
        toggleOff(false);
    }
    CCTextInputNode::onClickTrackNode(selected);
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
