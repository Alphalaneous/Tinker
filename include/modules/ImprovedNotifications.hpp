#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/TextAlertPopup.hpp>

class $module(ImprovedNotifications) {
    bool onToggled(bool state);
};

class $modify(INEditorUI, EditorUI) {
    $registerHooks(ImprovedNotifications);

};

class $modify(INTextAlertPopup, TextAlertPopup) {
    $registerHooks(ImprovedNotifications);

    bool init(std::string text, float delay, float scale, int opacity, std::string font);
};