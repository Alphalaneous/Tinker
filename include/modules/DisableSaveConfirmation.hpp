#pragma once

#include "module/Module.hpp"

class $module(DisableSaveConfirmation) {
    bool m_setSaveHijack = false;

    bool onToggled(bool state);
    void onEditor();

    void setHijack(EditorPauseLayer* pauseLayer);
};
