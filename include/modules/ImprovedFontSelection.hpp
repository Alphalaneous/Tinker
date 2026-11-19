#pragma once

#include "Geode/ui/TextInput.hpp"
#include "module/Module.hpp"
#include <Geode/modify/SelectFontLayer2.hpp>
#include "nodes/FontContainer.hpp"

class $module(ImprovedFontSelection) {
    bool onToggled(bool state);
    bool onSettingChanged(std::string_view key, const matjson::Value& value);
};

class $modify(IFSSelectFontLayer2, SelectFontLayer2) {
    $registerHooks(ImprovedFontSelection)
    
    struct Fields {
        geode::TextInput* m_exampleLabel = nullptr;
        std::vector<tinker::ui::FontContainer*> m_fontContainers{};
        int m_font;
    };

    std::string fontForID(int id);
    bool init(LevelEditorLayer* layer);
    void updateFont(int id);

    static geode::ZStringView fontNameForID(int id);
};