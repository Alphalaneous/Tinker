#pragma once

#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/cocos/cocoa/CCGeometry.h>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>
#include <Geode/cocos/sprite_nodes/CCSprite.h>
#include <Geode/utils/cocos.hpp>
#include <Geode/binding/GJGroundLayer.hpp>
#include <Geode/binding/GJMGLayer.hpp>
#include <ccTypes.h>
#include <alphalaneous.alphas-ui-pack/API.hpp>

namespace tinker::ui {

class PreviewGameLayer : public cocos2d::CCLayer {
public:
    static PreviewGameLayer* create(int background, int ground, int groundLine, int middleground);

    void updateBG(int background);
    void updateGround(int ground, int groundLine);
    void updateMiddleground(int middleground);

    void updateBGColor(const cocos2d::ccColor3B& color);
    void updateGroundColor(const cocos2d::ccColor3B& color1, const cocos2d::ccColor3B& color2, const cocos2d::ccColor3B& lineColor, GLubyte lineOpacity, bool lineBlend);
    void updateMiddlegroundColor(const cocos2d::ccColor3B& color1, GLubyte opacity1, bool blend1, const cocos2d::ccColor3B& color2, GLubyte opacity2, bool blend2);
protected:
    bool init(int background, int ground, int groundLine, int middleground);
    void update(float dt);

    cocos2d::CCSprite* m_backgroundSprite = nullptr;
    GJGroundLayer* m_groundLayer = nullptr;
    GJMGLayer* m_middleGround = nullptr;

    float m_backgroundWidth = 0.f;
    float m_bgOffset = 0.f;
    float m_groundOffset = 0.f;
    float m_mgOffset = 0.f;

    cocos2d::ccColor3B m_bgColor{};

    cocos2d::ccColor3B m_groundColor1{};
    cocos2d::ccColor3B m_groundColor2{};

    cocos2d::ccColor3B m_lineColor{};
    GLubyte m_lineOpacity = 0;
    bool m_lineBlend = false;

    cocos2d::ccColor3B m_mgColor1{};
    GLubyte m_mgOpacity1 = 0;
    bool m_mgBlend1 = false;
    
    cocos2d::ccColor3B m_mgColor2{};
    GLubyte m_mgOpacity2 = 0;
    bool m_mgBlend2 = false;
};
    
class ArtPreview : public cocos2d::CCNode {
public:
    static ArtPreview* create(cocos2d::CCSize size, int background, int ground, int groundLine, int middleground);

    void updateBG(int background);
    void updateGround(int ground, int groundLine);
    void updateMiddleground(int middleground);

    void updateBGColor(const cocos2d::ccColor3B& color);
    void updateGroundColor(const cocos2d::ccColor3B& color1, const cocos2d::ccColor3B& color2, const cocos2d::ccColor3B& lineColor, GLubyte lineOpacity, bool lineBlend);
    void updateMiddlegroundColor(const cocos2d::ccColor3B& color1, GLubyte opacity1, bool blend1, const cocos2d::ccColor3B& color2, GLubyte opacity2, bool blend2);
protected:
    bool init(cocos2d::CCSize size, int background, int ground, int groundLine, int middleground);

    geode::Ref<cocos2d::CCNode> m_container = nullptr;
    PreviewGameLayer* m_preview = nullptr;
    alpha::ui::RenderNode* m_render = nullptr;
};

}