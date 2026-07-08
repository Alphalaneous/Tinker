#pragma once

#include "module/Module.hpp"
#include <Geode/modify/SetupTriggerPopup.hpp>

class $editorModule(HideTriggerUI) {
    bool onToggled(bool state) override;
};

class $modify(HTUISetupTriggerPopup, SetupTriggerPopup) {
    $registerEditorHooks(HideTriggerUI)

    void sliderBegan(Slider* slider);
    void sliderEnded(Slider* slider);
};