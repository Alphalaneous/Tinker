#pragma once

#include "../Module.hpp"

class $editorModule(EditorGridCustomization) {
    static std::unordered_map<int, ccColor4B> s_triggerColors;
    
    void onEditor() override;
    bool onSettingChanged(std::string_view key, const matjson::Value& value) override;
};