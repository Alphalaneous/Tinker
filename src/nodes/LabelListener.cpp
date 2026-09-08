#include "nodes/LabelListener.hpp"

namespace tinker::ui {

LabelListener* LabelListener::create(ZStringView text, ZStringView font, geode::Function<void(ZStringView text)> callback) {
    auto ret = new LabelListener();
    if (ret->init(text, font, std::move(callback))) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool LabelListener::init(ZStringView text, ZStringView font, geode::Function<void(ZStringView text)> callback) {
    if (!CCLabelBMFont::initWithString(text.c_str(), font.c_str())) return false;
    m_callback = std::move(callback);

    if (m_callback) {
        m_callback(text);
    }

    return true;
}

void LabelListener::setString(const char* newString) {
    setString(newString, true);
}

void LabelListener::setString(const char* newString, bool needUpdateLabel) {
    CCLabelBMFont::setString(newString, needUpdateLabel);

    if (!m_blockCallback && m_callback) {
        m_blockCallback = true;
        m_callback(newString);
        m_blockCallback = false;
    }
}

}