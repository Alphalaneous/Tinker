#include "JoystickNavigation.hpp"
#include "NavigationControl.hpp"

using namespace tinker::ui;

bool JoystickNavigation::s_resetPosition = false;

void JoystickNavigation::onEditor() {
    auto navControl = NavigationControl::create(m_editorUI, getSetting<float, "opacity">(), getSetting<float, "scale">());
    navControl->setID("navigation-control"_spr);

    m_editorUI->m_uiItems->addObject(navControl);
    m_editorUI->addChild(navControl);

    s_resetPosition = false;
}

$on_mod(Loaded) {
    ButtonSettingPressedEventV3(Mod::get(), "JoystickNavigation-reset-position").listen([] (std::string_view btn) {
        createQuickPopup("Reset Controller Position?", "This will reset it to the default position relative to the editor UI.", "Cancel", "Yes", [] (auto alert, auto btn2) {
            if (btn2) JoystickNavigation::s_resetPosition = true;
        });
    }).leak();
}