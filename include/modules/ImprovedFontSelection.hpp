#pragma once

#include "module/Module.hpp"
#include <Geode/modify/SelectFontLayer.hpp>
#include "nodes/FontContainer.hpp"

class $module(ImprovedFontSelection) {
    bool onToggled(bool state);
    bool onSettingChanged(std::string_view key, const matjson::Value& value);
};

class $modify(IFSSelectFontLayer, SelectFontLayer) {
    $registerHooks(ImprovedFontSelection)
    
    struct Fields {
        cocos2d::CCLabelBMFont* m_exampleLabel = nullptr;
        std::vector<tinker::ui::FontContainer*> m_fontContainers{};
    };

    std::string fontForID(int id);
    bool init(LevelEditorLayer* layer);
    void updateFont(int id);

    static geode::ZStringView fontNameForID(int id);
};