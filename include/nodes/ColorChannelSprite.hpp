#pragma once

#include <Geode/cocos/sprite_nodes/CCSprite.h>
#include <Geode/ui/Label.hpp>
#include <string>

namespace tinker::ui {

class ColorChannelSprite : public cocos2d::CCSprite {
public:
    static ColorChannelSprite* create(int colorID, bool showLabel = true);
    void setSelected(bool selected);
    void setEnabled(bool enabled);
    void updateSprite();

    bool isSelected();

    int getColorID();
    void setColorID(int colorID);

    void setLive(bool live);
    bool isLive();

    std::string textForID(int colorID, bool any);
protected:
    bool init(int colorID, bool showLabel);

    geode::Label* m_copyLabel = nullptr;
    geode::Label* m_opacityLabel = nullptr;
    geode::Label* m_blendingLabel = nullptr;
    geode::Label* m_idLabel = nullptr;
    cocos2d::CCSprite* m_selectSpr = nullptr;

    bool m_enabled = true;
    bool m_selected = false;
    bool m_live = false;
    GLubyte m_realOpacity = 0;

    int m_colorID = 0;
};

};