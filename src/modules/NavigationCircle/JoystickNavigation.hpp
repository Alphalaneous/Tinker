#pragma once

#include "../../Module.hpp"

class $editorModule(JoystickNavigation) {
    static bool s_resetPosition;
    
    void onEditor() override;
};