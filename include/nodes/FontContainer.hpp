#pragma once

#include <Geode/ui/Label.hpp>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>
#include <Geode/utils/ZStringView.hpp>
#include <Geode/binding/CCMenuItemToggler.hpp>
#include <Geode/binding/SelectFontLayer.hpp>
#include <string>

namespace tinker::ui {

class FontContainer : public cocos2d::CCLayerColor {
public:
    static FontContainer* create(int id, float width, geode::ZStringView text, geode::ZStringView fontFile, SelectFontLayer2* fontLayer);

    void setVisible(bool visible);
    void toggle(bool toggle);
    int getFontID();

protected:
    bool init(int id, float width, geode::ZStringView text, geode::ZStringView fontFile, SelectFontLayer2* fontLayer);

    int m_id = 0;
    SelectFontLayer2* m_fontLayer = nullptr;
    geode::Label* m_label = nullptr;
    CCMenuItemToggler* m_toggle = nullptr;
    std::string m_text = "";
    std::string m_fontFile = "";
};

}