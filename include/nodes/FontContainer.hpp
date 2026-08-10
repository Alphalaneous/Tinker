#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace tinker::ui {

class FontContainer : public CCLayerColor {
public:
    static FontContainer* create(int id, float width, ZStringView text, ZStringView fontFile, SelectFontLayer* fontLayer);

    void setVisible(bool visible);
    void toggle(bool toggle);
    int getFontID();

protected:
    bool init(int id, float width, ZStringView text, ZStringView fontFile, SelectFontLayer* fontLayer);

    int m_id;
    SelectFontLayer* m_fontLayer;
    CCLabelBMFont* m_label;
    CCMenuItemToggler* m_toggle;
    std::string m_text;
    std::string m_fontFile;
};

}