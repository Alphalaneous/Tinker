#pragma once

#include "../Module.hpp"

class $editorModule(ThatPasteButton) {
    bool onToggled(bool state) override;
    bool onSettingChanged(std::string_view key, const matjson::Value& value) override;

    bool isGroupDefault(GameObject* object);
    bool areObjectGroupsDefault();

    bool isColorDefault(GameObject* object);
    bool areObjectColorsDefault();

    void onEditor() override;
    void onSetGroupIDLayer(SetGroupIDLayer* setGroupIDLayer, GameObject* obj, CCArray* objs) override;
};