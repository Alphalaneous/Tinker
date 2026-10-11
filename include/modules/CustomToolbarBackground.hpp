#pragma once

#include "module/Module.hpp"

class $module(CustomToolbarBackground) {
    cocos2d::CCLayerGradient* m_gradient = nullptr;
    cocos2d::CCLayerColor* m_line = nullptr;
    cocos2d::CCRect m_oldRect{};

    void onEditor();
    bool onToggled(bool state);
    bool onSettingChanged(std::string_view key, const matjson::Value& value);
};