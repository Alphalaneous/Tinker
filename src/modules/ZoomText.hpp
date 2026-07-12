#pragma once

#include "module/Module.hpp"

class $editorModule(ZoomText) {
    CCLabelBMFont* m_zoomLabel;

    bool onToggled(bool state) override;
    void onEditor() override;
    void showZoomText();
};