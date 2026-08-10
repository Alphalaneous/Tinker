#pragma once

#include "module/Module.hpp"

class $module(LDMObjectCount) {

    bool onToggled(bool state);
    void onEditor();

    void revertLabel(EditorPauseLayer* pauseLayer);
    void setLabel(EditorPauseLayer* pauseLayer);
};
