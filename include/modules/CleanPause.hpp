#pragma once

#include "module/Module.hpp"

class $module(CleanPause) {
    void onEditor();

    static float volumeToSlider(float n);
    static float sliderToVolume(float n);

    void resizeButton(CCNode* button, float width);
    void scaleAndPosition(EditorPauseLayer* pauseLayer, float scale);
};
