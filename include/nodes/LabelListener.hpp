#pragma once

#include <Geode/cocos/label_nodes/CCLabelBMFont.h>
#include <Geode/utils/ZStringView.hpp>
#include <Geode/utils/function.hpp>

namespace tinker::ui {

class LabelListener : public cocos2d::CCLabelBMFont {
public:
    static LabelListener* create(geode::ZStringView text, geode::ZStringView font, geode::Function<void(geode::ZStringView text)> callback);

    void setString(const char* newString) override;
    void setString(const char* newString, bool needUpdateLabel) override;
protected:
    bool init(geode::ZStringView text, geode::ZStringView font, geode::Function<void(geode::ZStringView text)> callback);

    bool m_blockCallback = false;
    geode::Function<void(geode::ZStringView text)> m_callback = nullptr;
};

}