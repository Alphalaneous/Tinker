#pragma once

#include "module/Module.hpp"
#include "nodes/NavigationControl.hpp"

class $module(JoystickNavigation) {
    static bool s_resetPosition;
    
    tinker::ui::NavigationControl* m_navigationControl;
    
    void onEditor();

    bool onToggled(bool state);
    bool onSettingChanged(std::string_view key, const matjson::Value& value);

    void updateController(bool canvasRotate);
};