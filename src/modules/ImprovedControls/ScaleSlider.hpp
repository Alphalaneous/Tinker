#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/SliderNode.hpp>

using namespace geode::prelude;

namespace tinker::ui {

class ScaleSlider : public SliderNode {
public:
    using ScaleSliderCallback = geode::Function<void(ScaleSlider* sender, float value)>;

    static ScaleSlider* create(ScaleSliderCallback callback, GJScaleControl* control);

    void updateSnap(float snap);
    void setValue(float value, bool skipCallback = false);
    void setPercent(float value, bool skipCallback = false);

    CCSprite* getLargeTick();
    CCSprite* getSmallTick();

    void clearExtendedGroove();

    float valueToLocalX(float value);
    float sliderToExtendedGrooveX(float x);

    void addTick(CCSprite* spr, float sliderLocalX, float grooveEdgeX, bool isRightSide, const ccColor3B& insideColor, GLubyte insideOpacity, const ccColor3B& outsideColor, GLubyte outsideOpacity);
    void sweepTicks(int start, int step, int limit, float edgeX, float grooveEdgeX, bool isRightSide, const ccColor3B& extendedColor);

    virtual bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;

protected:
    bool init(ScaleSliderCallback callback, GJScaleControl* control);
    void updateExtendedGroove();

    GJScaleControl* m_scaleControl;
    std::vector<Ref<CCSprite>> m_smallTicks;
    std::vector<Ref<CCSprite>> m_largeTicks;
    int m_smallTicksRemaining;
    int m_largeTicksRemaining;
    
    float m_snap = 4.f;
    bool m_skipCallback;
    bool m_lock;
    CCNode* m_snapPointContainer;
    NineSlice* m_extendedGroove;
};

}