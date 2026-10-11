#pragma once

#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/ui/NineSlice.hpp>
#include <ccTypes.h>

namespace tinker::ui {

class PopupBorder : public cocos2d::CCNode {
public:
    static PopupBorder* create(const cocos2d::ccColor3B& topColor, GLubyte topOpacity, const cocos2d::ccColor3B& bottomColor, GLubyte bottomOpacity);
    void setContentSize(const cocos2d::CCSize& contentSize);

    void hideLeft(bool hide);
    void hideRight(bool hide);
    void hideTop(bool hide);
    void hideBottom(bool hide);
protected:
    bool init(const cocos2d::ccColor3B& topColor, GLubyte topOpacity, const cocos2d::ccColor3B& bottomColor, GLubyte bottomOpacity);

    geode::NineSlice* m_border = nullptr;
    geode::NineSlice* m_innerTopBorder = nullptr;
    geode::NineSlice* m_innerBottomBorder = nullptr;

    bool m_hideLeft = false;
    bool m_hideRight = false;
    bool m_hideTop = false;
    bool m_hideBottom = false;

};

}