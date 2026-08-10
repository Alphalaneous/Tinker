#pragma once

#include "module/Module.hpp"
#include <Geode/modify/SelectFontLayer.hpp>
#include "nodes/FontContainer.hpp"

class $module(ImprovedFontSelection) {
    bool onToggled(bool state);
    bool onSettingChanged(std::string_view key, const matjson::Value& value);

    tinker::utils::ScopedHookToggle<"hjfod.betteredit", ImprovedFontSelection> m_toggledHooks = {
        "SelectFontLayer::init"
    };
};

class $modify(IFSSelectFontLayer, SelectFontLayer) {
    $registerHooks(ImprovedFontSelection)
    
    struct Fields {
        CCLabelBMFont* m_exampleLabel;
        std::vector<tinker::ui::FontContainer*> m_fontContainers;
    };

    std::string fontForID(int id);
    bool init(LevelEditorLayer* layer);
    void updateFont(int id);

    static ZStringView fontNameForID(int id);
};