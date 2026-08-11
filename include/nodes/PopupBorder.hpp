#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace tinker::ui {

class PopupBorder : public CCNode {
public:
    static PopupBorder* create(const ccColor3B& topColor, GLubyte topOpacity, const ccColor3B& bottomColor, GLubyte bottomOpacity);
    void setContentSize(const CCSize& contentSize);

    void hideLeft(bool hide);
    void hideRight(bool hide);
    void hideTop(bool hide);
    void hideBottom(bool hide);
protected:
    bool init(const ccColor3B& topColor, GLubyte topOpacity, const ccColor3B& bottomColor, GLubyte bottomOpacity);


    geode::NineSlice* m_border;
    geode::NineSlice* m_innerTopBorder;
    geode::NineSlice* m_innerBottomBorder;

    bool m_hideLeft;
    bool m_hideRight;
    bool m_hideTop;
    bool m_hideBottom;

};

}