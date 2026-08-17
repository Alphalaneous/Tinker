#pragma once

#include "module/Module.hpp"

class $module(CustomToolbarBackground) {
    CCLayerGradient* m_gradient;
    CCLayerColor* m_line;
    CCRect m_oldRect;

    void onEditor();
    bool onToggled(bool state);
    bool onSettingChanged(std::string_view key, const matjson::Value& value);
};