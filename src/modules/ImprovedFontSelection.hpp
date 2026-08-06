#pragma once

#include "module/Module.hpp"
#include <Geode/modify/SelectFontLayer.hpp>

class $editorModule(ImprovedFontSelection) {
    bool onToggled(bool state) override;
    bool onSettingChanged(std::string_view key, const matjson::Value& value) override;

    tinker::utils::ScopedHookToggle<"hjfod.betteredit", ImprovedFontSelection> m_toggledHooks = {
        "SelectFontLayer::init"
    };
};

class FontContainer;

class $modify(IFSSelectFontLayer, SelectFontLayer) {
    $registerEditorHooks(ImprovedFontSelection)
    
    struct Fields {
        CCLabelBMFont* m_exampleLabel;
        std::vector<FontContainer*> m_fontContainers;
    };

    std::string fontForID(int id);
    bool init(LevelEditorLayer* layer);
    void updateFont(int id);

    static ZStringView fontNameForID(int id);
};

class FontContainer : public CCLayerColor {
public:
    static FontContainer* create(int id, float width, ZStringView text, ZStringView fontFile, IFSSelectFontLayer* fontLayer);

    void setVisible(bool visible);
    void toggle(bool toggle);
    int getFontID();

protected:
    bool init(int id, float width, ZStringView text, ZStringView fontFile, IFSSelectFontLayer* fontLayer);

    int m_id;
    IFSSelectFontLayer* m_fontLayer;
    CCLabelBMFont* m_label;
    CCMenuItemToggler* m_toggle;
    std::string m_text;
    std::string m_fontFile;
};