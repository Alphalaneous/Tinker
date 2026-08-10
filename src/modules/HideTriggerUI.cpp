#include "modules/HideTriggerUI.hpp"

bool HideTriggerUI::onToggled(bool state) {
    return true;
}

void HTUISetupTriggerPopup::sliderBegan(Slider* slider) {
    SetupTriggerPopup::sliderBegan(slider);
    
    auto bg =  m_mainLayer->getChildByType<CCScale9Sprite>(0);
    if (bg) {
        bg->stopAllActions();
        bg->runAction(CCFadeTo::create(0.15f, 0));
    }
    stopAllActions();
    runAction(CCFadeTo::create(0.15f, 0));
}

void HTUISetupTriggerPopup::sliderEnded(Slider* slider) {
    SetupTriggerPopup::sliderEnded(slider);

    auto bg =  m_mainLayer->getChildByType<CCScale9Sprite>(0);
    if (bg) {
        bg->stopAllActions();
        bg->runAction(CCFadeTo::create(0.15f, 255));
    }
    stopAllActions();
    runAction(CCFadeTo::create(0.15f, 150));
}