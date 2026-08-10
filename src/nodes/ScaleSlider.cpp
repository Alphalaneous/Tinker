#include "nodes/ScaleSlider.hpp"
#include "modules/ImprovedControls.hpp"

namespace tinker::ui {

ScaleSlider* ScaleSlider::create(ScaleSliderCallback callback, CCNode* control) {
    auto ret = new ScaleSlider();
    if (ret->init(std::move(callback), control)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool ScaleSlider::init(ScaleSliderCallback callback, CCNode* control) {
    if (!SliderNode::initCustom(
        CCSprite::create("GJ_moveBtn.png"),
        CCSprite::create("GJ_moveSBtn.png"),
        NineSlice::create("geode.loader/slider-groove-2.png"),
        "sliderBar.png",
        [this, callback = std::move(callback)] (SliderNode* sender, float value) mutable {
            updateExtendedGroove();
            if (m_skipCallback) return;

            if (auto scaleControl = typeinfo_cast<GJScaleControl*>(m_control)) {
                auto scale = static_cast<ICGJScaleControl*>(scaleControl)->trueValueFromScale(value);
                if (scale > 0.97f && scale < 1.03f) {
                    value = static_cast<ICGJScaleControl*>(scaleControl)->trueValueFromScale(1);
                }
            }

            if (callback) callback(static_cast<ScaleSlider*>(sender), value);
        },
        {2.f, 2.f}
    )) return false;

    setMin(0.f);
    setMax(1.f);
    getBar()->setVisible(false);
    setContentWidth(210.f);
    setSliderBypass(true);

    m_control = control;

    m_extendedGroove = NineSlice::create("geode.loader/slider-groove-2.png");
    m_extendedGroove->setID("extended-groove"_spr);
    m_extendedGroove->setOpacity(127);
    m_extendedGroove->setZOrder(-10);
    m_extendedGroove->setContentSize(getGroove()->getContentSize());
    m_extendedGroove->setPosition(getGroove()->getPosition());

    addChild(m_extendedGroove);

    m_snapPointContainer = CCNode::create();
    m_snapPointContainer->setZOrder(-10);
    m_snapPointContainer->setID("snap-point-container"_spr);
    m_snapPointContainer->setAnchorPoint({0.f, 0.5f});
    m_snapPointContainer->setContentSize(getGroove()->getContentSize());
    m_snapPointContainer->setPosition(getGroove()->getPosition());

    addChild(m_snapPointContainer);

    getGroove()->setID("groove"_spr);
    getBar()->setID("bar"_spr);
    getThumb()->setID("thumb"_spr);

    updateExtendedGroove();

    return true;
}

void ScaleSlider::updateExtendedGroove() {
    auto percent = getPercent();
    if (percent < 0.f) {
        m_extendedGroove->setAnchorPoint({1.f, 0.5f});
        m_extendedGroove->setContentWidth(getGroove()->getContentWidth() * (1 - percent));
        m_extendedGroove->setPositionX(getGroove()->getContentWidth());
        m_extendedGroove->setColor({255, 127, 127});
    }
    else {
        m_extendedGroove->setAnchorPoint({0.f, 0.5f});
        m_extendedGroove->setContentWidth(getGroove()->getContentWidth() * percent);
        m_extendedGroove->setPositionX(0.f);
        m_extendedGroove->setColor({127, 255, 127});
    }
    m_extendedGroove->setVisible(percent < 0.f || percent > 1.f);
    updateSnap(m_snap);
}

bool ScaleSlider::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    if (!nodeIsVisible(this)) return false;

    if (auto scaleControl = typeinfo_cast<GJScaleControl*>(m_control)) {
        if (scaleControl->m_delegate) {
            scaleControl->m_delegate->scaleChangeBegin();
        }
    }


    return SliderNode::ccTouchBegan(touch, event);
}

void ScaleSlider::disableBypass() {
    setSliderBypass(false);
}

void ScaleSlider::setValue(float value, bool skipCallback) {
    m_skipCallback = skipCallback;
    SliderNode::setValue(value);
    m_skipCallback = false;
    updateExtendedGroove();
}

void ScaleSlider::setPercent(float value, bool skipCallback) {
    m_skipCallback = skipCallback;
    SliderNode::setPercent(value);
    m_skipCallback = false;
    updateExtendedGroove();
}

CCSprite* ScaleSlider::getLargeTick() {
    CCSprite* spr;
    if (m_largeTicksRemaining == 0) {
        spr = CCSprite::create("slider-tick.png"_spr);
        m_largeTicks.push_back(spr);
    }
    else {
        spr = m_largeTicks[m_largeTicksRemaining - 1];
        m_largeTicksRemaining--;
    }
    return spr;
}

CCSprite* ScaleSlider::getSmallTick() {
    if (m_smallTicksRemaining == 0) {
        auto spr = CCSprite::create("slider-tick-small.png"_spr);
        m_smallTicks.push_back(spr);
        return spr;
    }
    auto spr = m_smallTicks[m_smallTicksRemaining - 1];
    m_smallTicksRemaining--;
    return spr;
}

void ScaleSlider::sweepTicks(int step, int limit, float edgeX, float grooveEdgeX, const ccColor3B& extendedColor) {
    int i = 1;
    bool isRightSide = step == 1;
    while (true) {
        float posX = valueToLocalX(i);
        bool pastEdge = isRightSide ? (posX > edgeX) : (posX < edgeX);

        for (int j = 1; j < m_snap; j++) {
            float smallValue = isRightSide ? i + (j * (1.f / m_snap)) : i - (j * (1.f / m_snap));
            float smallX = valueToLocalX(smallValue);

            bool smallPastEdge = isRightSide ? (smallX > edgeX) : (smallX < edgeX);
            if (smallPastEdge) break;
            
            if (smallX > 2.f || smallX < -2.f) {
                addTick(getSmallTick(), smallX, grooveEdgeX, isRightSide, {255, 255, 255}, 105, extendedColor, 52);
            }
        }

        if (pastEdge) break;
        
        if (!isRightSide && i == 1) {
            i--;
            continue;
        }

        if (posX > 2.f || posX < -2.f) {
            addTick(getLargeTick(), posX, grooveEdgeX, isRightSide, {255, 255, 255}, 255, extendedColor, 127);
        }

        i += step;
        if ((isRightSide && i > limit) || (!isRightSide && i < limit)) {
            break;
        }
    }
}

void ScaleSlider::addTick(CCSprite* spr, float sliderLocalX, float grooveEdgeX, bool isRightSide, const ccColor3B& insideColor, GLubyte insideOpacity, const ccColor3B& outsideColor, GLubyte outsideOpacity) {
    spr->setColor(insideColor);
    spr->setOpacity(insideOpacity);

    bool isOutsideGroove = isRightSide ? (sliderLocalX > grooveEdgeX) : (sliderLocalX < grooveEdgeX);

    if (isOutsideGroove) {
        spr->setColor(outsideColor);
        spr->setOpacity(outsideOpacity);
        spr->setPosition({sliderToExtendedGrooveX(sliderLocalX), m_extendedGroove->getContentSize().height / 2.f});
        m_extendedGroove->addChild(spr);
    } 
    else {
        spr->setPosition({sliderLocalX - m_snapPointContainer->getPositionX(), m_snapPointContainer->getContentSize().height / 2.f});
        m_snapPointContainer->addChild(spr);
    }
}

float ScaleSlider::sliderToExtendedGrooveX(float x) {
    if (x > 0) return x - m_extendedGroove->getPositionX();
    else return m_extendedGroove->getContentWidth() - getGroove()->getContentWidth() + x;
}

float ScaleSlider::valueToLocalX(float value) {
    float lower = 0.f;
    float upper = 1.f;

    if (auto scaleControl = typeinfo_cast<GJScaleControl*>(m_control)) {
        lower = scaleControl->m_lowerBound;
        upper = scaleControl->m_upperBound;

        if (ImprovedControls::getSetting<bool, "custom-scale-min-max">()) {
            lower = ImprovedControls::getSetting<float, "scale-min">();
            upper = ImprovedControls::getSetting<float, "scale-max">();

            if (lower > upper) {
                std::swap(lower, upper);
            }
        }
    }

    float range = upper - lower;

    if (range == 0.f) return 0.f;

    float t = (value - lower) / range;
    return getContentSize().width * t;
}

void ScaleSlider::updateSnap(float snap) {
    m_snapPointContainer->removeAllChildren();
    clearExtendedGroove();

    m_smallTicksRemaining = m_smallTicks.size();
    m_largeTicksRemaining = m_largeTicks.size();
    m_snap = snap;

    const float grooveWidth = getGroove()->getContentSize().width;
    const float grooveRightX = grooveWidth - 2.f;
    const float grooveLeftX = 2.f;

    const float rightEdgeX = std::max(grooveWidth, grooveWidth * getPercent()) - 2.f;
    const float leftEdgeX = std::min(0.f, grooveWidth * getPercent()) + 2.f;

    sweepTicks(1, 100, rightEdgeX, grooveRightX, {127, 255, 127});
    sweepTicks(-1, -100, leftEdgeX, grooveLeftX, {255, 127, 127});
}

void ScaleSlider::clearExtendedGroove() {
    for (int i = m_extendedGroove->getChildrenCount() - 1; i >= 0; i--) {
        m_extendedGroove->getChildrenExt()[i]->removeFromParent();
    }
}

}