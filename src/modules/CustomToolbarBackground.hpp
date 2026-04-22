#pragma once

#include "../Module.hpp"

class $editorModule(CustomToolbarBackground) {
    CCLayerGradient* m_gradient;
    CCLayerColor* m_line;

    void onEditor() override;
    bool onSettingChanged(std::string_view key, const matjson::Value& value) override;
};