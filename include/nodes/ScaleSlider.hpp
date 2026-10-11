#pragma once

#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/cocos/sprite_nodes/CCSprite.h>
#include <Geode/cocos/touch_dispatcher/CCTouch.h>
#include <Geode/ui/NineSlice.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/utils/function.hpp>
#include <Geode/ui/SliderNode.hpp>
#include <ccTypes.h>
#include <vector>

namespace tinker::ui {

class ScaleSlider : public geode::SliderNode {
public:
    using ScaleSliderCallback = geode::Function<void(ScaleSlider* sender, float value)>;

    static ScaleSlider* create(ScaleSliderCallback callback, cocos2d::CCNode* control);

    void updateSnap(float snap);
    void setValue(float value, bool skipCallback = false);
    void setPercent(float value, bool skipCallback = false);

    cocos2d::CCSprite* getLargeTick();
    cocos2d::CCSprite* getSmallTick();

    void clearExtendedGroove();
    void disableBypass();

    float valueToLocalX(float value);
    float sliderToExtendedGrooveX(float x);

    void addTick(cocos2d::CCSprite* spr, float sliderLocalX, float grooveEdgeX, bool isRightSide, const cocos2d::ccColor3B& insideColor, GLubyte insideOpacity, const cocos2d::ccColor3B& outsideColor, GLubyte outsideOpacity);
    void sweepTicks(int step, int limit, float edgeX, float grooveEdgeX, const cocos2d::ccColor3B& extendedColor);

    virtual bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;

protected:
    bool init(ScaleSliderCallback callback, cocos2d::CCNode* control);
    void updateExtendedGroove();

    cocos2d::CCNode* m_control = nullptr;
    std::vector<geode::Ref<cocos2d::CCSprite>> m_smallTicks{};
    std::vector<geode::Ref<cocos2d::CCSprite>> m_largeTicks{};
    int m_smallTicksRemaining = 0;
    int m_largeTicksRemaining = 0;
    
    float m_snap = 4.f;
    bool m_skipCallback = false;
    bool m_lock = false;
    cocos2d::CCNode* m_snapPointContainer = nullptr;
    geode::NineSlice* m_extendedGroove = nullptr;
};

}