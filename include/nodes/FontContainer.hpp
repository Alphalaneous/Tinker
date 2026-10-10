#pragma once

#include <Geode/ui/Label.hpp>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>
#include <Geode/utils/ZStringView.hpp>
#include <Geode/binding/CCMenuItemToggler.hpp>
#include <Geode/binding/SelectFontLayer.hpp>
#include <string>

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
    geode::Label* m_label;
    CCMenuItemToggler* m_toggle;
    std::string m_text;
    std::string m_fontFile;
};

}