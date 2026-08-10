#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace tinker::ui {

class ColorChannelSprite : public CCSprite {
public:
    static ColorChannelSprite* create(int colorID, bool showLabel = true);
    void setSelected(bool selected);
    void setEnabled(bool enabled);
    void updateSprite();

    int getColorID();
    void setColorID(int colorID);

    void setLive(bool live);
    bool isLive();

    std::string textForID(int colorID, bool any);
protected:
    bool init(int colorID, bool showLabel);

    cocos2d::CCLabelBMFont* m_copyLabel;
    cocos2d::CCLabelBMFont* m_opacityLabel;
    cocos2d::CCLabelBMFont* m_blendingLabel;
    cocos2d::CCLabelBMFont* m_idLabel;
    CCSprite* m_selectSpr;

    bool m_enabled = true;
    bool m_live;
    GLubyte m_realOpacity;

    int m_colorID;
};

};