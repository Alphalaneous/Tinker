#pragma once

#include "module/Module.hpp"

class SliderBypassTest : public CCNode {
public:
    static SliderBypassTest* create();
    bool hasBypass();
    void onSlider(CCObject* sender);
protected:
    bool init();

    bool m_hasBypass;
};

class $module(CleanPause) {
    void onEditor();

    static float volumeToSlider(float n);
    static float sliderToVolume(float n);

    void resizeButton(CCNode* button, float width);
    void scaleAndPosition(EditorPauseLayer* pauseLayer, float scale);

    bool isSliderBypassEnabled();
};
