#pragma once

#include "module/Module.hpp"

class $module(ThatPasteButton) {
    bool onToggled(bool state);
    bool onSettingChanged(std::string_view key, const matjson::Value& value);

    bool isGroupDefault(GameObject* object);
    bool areObjectGroupsDefault();

    bool isColorDefault(GameObject* object);
    bool areObjectColorsDefault();

    void onEditor();
};