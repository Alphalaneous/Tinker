#pragma once

#include "module/Module.hpp"

class $editorModule(LDMObjectCount) {

    bool onToggled(bool state) override;
    void onEditor() override;

    void revertLabel(EditorPauseLayer* pauseLayer);
    void setLabel(EditorPauseLayer* pauseLayer);
};
