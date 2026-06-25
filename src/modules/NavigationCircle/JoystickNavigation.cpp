#include "JoystickNavigation.hpp"
#include "NavigationControl.hpp"
#include "modules/CanvasRotate/CanvasRotate.hpp"

using namespace tinker::ui;

bool JoystickNavigation::s_resetPosition = false;

bool JoystickNavigation::onToggled(bool state) {
    if (state) {
        onEditor();
    }
    else {
        if (m_navigationControl) {
            m_editorUI->m_uiItems->removeObject(m_navigationControl);
            m_navigationControl->removeFromParent();
        }
    }
    return true;
}

void JoystickNavigation::updateController(bool canvasRotate) {
    if (m_navigationControl) {
        m_navigationControl->updateControl(canvasRotate);
    }
}

bool JoystickNavigation::onSettingChanged(std::string_view key, const matjson::Value& value) {
    updateController(CanvasRotate::isEnabled());
    return true;
}

void JoystickNavigation::onEditor() {
    m_navigationControl = NavigationControl::create(m_editorUI);
    m_navigationControl->setID("navigation-control"_spr);

    m_editorUI->m_uiItems->addObject(m_navigationControl);
    m_editorUI->addChild(m_navigationControl);

    s_resetPosition = false;
}

$on_mod(Loaded) {
    ButtonSettingPressedEventV3(Mod::get(), "JoystickNavigation-reset-position").listen([] (std::string_view btn) {
        createQuickPopup("Reset Controller Position?", "This will reset it to the default position relative to the editor UI.", "Cancel", "Yes", [] (auto alert, auto btn2) {
            if (btn2) JoystickNavigation::s_resetPosition = true;
        });
    }).leak();
}