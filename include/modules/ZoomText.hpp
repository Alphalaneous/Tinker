#pragma once

#include "module/Module.hpp"

class $module(ZoomText) {
    geode::Label* m_zoomLabel;

    bool onToggled(bool state);
    void onEditor();
    void showZoomText();
};