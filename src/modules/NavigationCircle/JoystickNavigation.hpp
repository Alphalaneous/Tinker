#pragma once

#include "module/Module.hpp"

namespace tinker::ui {
    class NavigationControl;
}

class $editorModule(JoystickNavigation) {
    static bool s_resetPosition;
    
    tinker::ui::NavigationControl* m_navigationControl;
    
    void onEditor() override;

    bool onToggled(bool state) override;
    bool onSettingChanged(std::string_view key, const matjson::Value& value) override;

    void updateController(bool canvasRotate);
};