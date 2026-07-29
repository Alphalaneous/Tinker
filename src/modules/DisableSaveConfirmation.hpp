#pragma once

#include "module/Module.hpp"

class $editorModule(DisableSaveConfirmation) {
    bool m_setSaveHijack = false;

    bool onToggled(bool state) override;
    void onEditor() override;

    void setHijack(EditorPauseLayer* pauseLayer);
};
