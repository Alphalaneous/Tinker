#pragma once

#include <Geode/Geode.hpp>
#include <alphalaneous.alphas-ui-pack/include/API.hpp>

using namespace geode::prelude;

namespace tinker::ui {

class PreviewGameLayer : public CCLayer {
public:
    static PreviewGameLayer* create(int background, int ground, int groundLine, int middleground);

    void updateBG(int background);
    void updateGround(int ground, int groundLine);
    void updateMiddleground(int middleground);

    void updateBGColor(const ccColor3B& color);
    void updateGroundColor(const ccColor3B& color1, const ccColor3B& color2, const ccColor3B& lineColor, GLubyte lineOpacity, bool lineBlend);
    void updateMiddlegroundColor(const ccColor3B& color1, GLubyte opacity1, bool blend1, const ccColor3B& color2, GLubyte opacity2, bool blend2);
protected:
    bool init(int background, int ground, int groundLine, int middleground);
    void update(float dt);

    cocos2d::CCSprite* m_backgroundSprite;
    GJGroundLayer* m_groundLayer;
    GJMGLayer* m_middleGround;

    float m_backgroundWidth;
    float m_bgOffset;
    float m_groundOffset;
    float m_mgOffset;

    ccColor3B m_bgColor;

    ccColor3B m_groundColor1;
    ccColor3B m_groundColor2;

    ccColor3B m_lineColor;
    GLubyte m_lineOpacity;
    bool m_lineBlend;

    ccColor3B m_mgColor1;
    GLubyte m_mgOpacity1;
    bool m_mgBlend1;
    
    ccColor3B m_mgColor2;
    GLubyte m_mgOpacity2;
    bool m_mgBlend2;
};
    
class ArtPreview : public CCNode {
public:
    static ArtPreview* create(CCSize size, int background, int ground, int groundLine, int middleground);

    void updateBG(int background);
    void updateGround(int ground, int groundLine);
    void updateMiddleground(int middleground);

    void updateBGColor(const ccColor3B& color);
    void updateGroundColor(const ccColor3B& color1, const ccColor3B& color2, const ccColor3B& lineColor, GLubyte lineOpacity, bool lineBlend);
    void updateMiddlegroundColor(const ccColor3B& color1, GLubyte opacity1, bool blend1, const ccColor3B& color2, GLubyte opacity2, bool blend2);
protected:
    bool init(CCSize size, int background, int ground, int groundLine, int middleground);

    // I am sorry, but this fixes a crash from QOLMod
    cocos2d::CCArray* getChildren();
    unsigned int getChildrenCount() const;

    Ref<CCNode> m_container;
    PreviewGameLayer* m_preview;
    alpha::ui::RenderNode* m_render;
    Ref<CCArray> m_fakeChildren;
};

}