#pragma once

#include "module/Module.hpp"
#include <Geode/modify/SetupTriggerPopup.hpp>

class $module(HideTriggerUI) {
    bool onToggled(bool state);
};

class $modify(HTUISetupTriggerPopup, SetupTriggerPopup) {
    $registerHooks(HideTriggerUI)

    void sliderBegan(Slider* slider);
    void sliderEnded(Slider* slider);
};