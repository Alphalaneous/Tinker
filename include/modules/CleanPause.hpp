#pragma once

#include "module/Module.hpp"

class SliderBypassTest : public cocos2d::CCNode {
public:
    static SliderBypassTest* create();
    bool hasBypass();
    void onSlider(cocos2d::CCObject* sender);
protected:
    bool init();

    bool m_hasBypass = false;
};

class $module(CleanPause) {
    void onEditor();

    static float volumeToSlider(float n);
    static float sliderToVolume(float n);

    void resizeButton(cocos2d::CCNode* button, float width);
    void scaleAndPosition(EditorPauseLayer* pauseLayer, float scale);

    bool isSliderBypassEnabled();
};
