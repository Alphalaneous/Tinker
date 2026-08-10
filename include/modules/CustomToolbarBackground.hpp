#pragma once

#include "module/Module.hpp"

class $module(CustomToolbarBackground) {
    CCLayerGradient* m_gradient;
    CCLayerColor* m_line;

    void onEditor();
    bool onSettingChanged(std::string_view key, const matjson::Value& value);
};