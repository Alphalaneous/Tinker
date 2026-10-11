#pragma once

#include "module/Module.hpp"

class $module(ZoomText) {
    geode::Label* m_zoomLabel = nullptr;

    bool onToggled(bool state);
    void onEditor();
    void showZoomText();
    int getHighestChildZ();
};