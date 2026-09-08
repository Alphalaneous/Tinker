#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace tinker::ui {

class LabelListener : public CCLabelBMFont {
public:
    static LabelListener* create(ZStringView text, ZStringView font, geode::Function<void(ZStringView text)> callback);

    void setString(const char* newString) override;
    void setString(const char* newString, bool needUpdateLabel) override;
protected:
    bool init(ZStringView text, ZStringView font, geode::Function<void(ZStringView text)> callback);

    bool m_blockCallback;
    geode::Function<void(ZStringView text)> m_callback;
};

}