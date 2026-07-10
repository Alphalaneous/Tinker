#include "ScaleSlider.hpp"
#include "modules/ImprovedControls/ImprovedControls.hpp"

using namespace tinker::ui;

ScaleSlider* ScaleSlider::create(ScaleSliderCallback callback, GJScaleControl* control) {
    auto ret = new ScaleSlider();
    if (ret->init(std::move(callback), control)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool ScaleSlider::init(ScaleSliderCallback callback, GJScaleControl* control) {
    if (!SliderNode::initCustom(
        CCSprite::create("GJ_moveBtn.png"),
        CCSprite::create("GJ_moveSBtn.png"),
        NineSlice::create("geode.loader/slider-groove-2.png"),
        "sliderBar.png",
        [this, callback = std::move(callback)] (SliderNode* sender, float value) mutable {
            updateExtendedGroove();
            if (m_skipCallback) return;

            auto scale = m_scaleControl->scaleFromValue(value);
            if (scale > 0.97f && scale < 1.03f) {
                value = static_cast<ICGJScaleControl*>(m_scaleControl)->trueValueFromScale(1);
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

    m_scaleControl = control;

    m_extendedGroove = NineSlice::create("geode.loader/slider-groove-2.png");
    m_extendedGroove->setID("extended-groove"_spr);
    m_extendedGroove->setOpacity(127);
    m_extendedGroove->setZOrder(-10);
    m_extendedGroove->setContentSize(getGroove()->getContentSize());
    m_extendedGroove->setPosition(getGroove()->getPosition());

    addChild(m_extendedGroove);

    m_snapPointContainer = CCNode::create();
    m_snapPointContainer->setID("snap-point-container"_spr);
    m_snapPointContainer->setAnchorPoint({0.5f, 0.5f});
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

        m_snapPointContainer->setAnchorPoint({1.f, 0.5f});
        m_snapPointContainer->setContentSize(m_extendedGroove->getContentSize());
        m_snapPointContainer->setPositionX(getGroove()->getContentWidth());
    }
    else {
        m_extendedGroove->setAnchorPoint({0.f, 0.5f});
        m_extendedGroove->setContentWidth(getGroove()->getContentWidth() * percent);
        m_extendedGroove->setPositionX(0.f);
        m_extendedGroove->setColor({127, 255, 127});

        m_snapPointContainer->setAnchorPoint({0.f, 0.5f});
        m_snapPointContainer->setContentSize(m_extendedGroove->getContentSize());
        m_snapPointContainer->setPositionX(0.f);
    }
    m_extendedGroove->setVisible(percent < 0.f || percent > 1.f);
    updateSnap(m_snap);
}

bool ScaleSlider::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    if (!nodeIsVisible(this)) return false;

    return SliderNode::ccTouchBegan(touch, event);
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

void ScaleSlider::updateSnap(float snap) {
    m_snapPointContainer->removeAllChildren();
    for (int i = m_extendedGroove->getChildrenCount() - 1; i >= 0; i--) {
        m_extendedGroove->getChildrenExt()[i]->removeFromParent();
    }

    m_snap = snap;

    auto rightGrooveEdgeWorld = convertToWorldSpace({getGroove()->getContentWidth() - 1.f, 0.f});
    auto rightGrooveEdgeInContainer = m_snapPointContainer->convertToNodeSpace(rightGrooveEdgeWorld);

    auto rightEdgeWorld = convertToWorldSpace({std::max(getGroove()->getContentWidth(), getGroove()->getContentWidth() * getPercent()) - 1.f, 0.f});
    auto rightEdgeInContainer = m_snapPointContainer->convertToNodeSpace(rightEdgeWorld);

    int i = 1;
    while (true) {
        auto value = (i - m_scaleControl->m_lowerBound) / (m_scaleControl->m_upperBound - m_scaleControl->m_lowerBound);
        auto posWorld = convertToWorldSpace({getContentWidth() * value, 0.f});
        auto posInContainer = m_snapPointContainer->convertToNodeSpace(posWorld);

        for (int j = 1; j < m_snap; j++) {
            auto smallValue = (i + (j * (1 / m_snap)) - m_scaleControl->m_lowerBound) / (m_scaleControl->m_upperBound - m_scaleControl->m_lowerBound);
            auto smallPosWorld = convertToWorldSpace({getContentWidth() * smallValue, 0.f});
            auto smallPosInContainer = m_snapPointContainer->convertToNodeSpace(smallPosWorld);

            if (smallPosInContainer.x > rightEdgeInContainer.x) {
                break;
            }

            auto spr = CCSprite::create("slider-tick-small.png"_spr);
            spr->setOpacity(105);
            spr->setID(fmt::format("snap-tick-small-{}"_spr, numToString(smallValue, 3)));

            if (smallPosInContainer.x > rightGrooveEdgeInContainer.x) {
                spr->setColor({127, 255, 127});
                spr->setOpacity(52);
                auto posInExtendedGroove = m_extendedGroove->convertToNodeSpace(smallPosWorld);
                spr->setPosition({posInExtendedGroove.x, m_extendedGroove->getContentHeight() / 2.f});
                m_extendedGroove->addChild(spr);
            }
            else {
                spr->setPosition({smallPosInContainer.x, m_snapPointContainer->getContentHeight() / 2.f});
                m_snapPointContainer->addChild(spr);
            }
        }

        if (posInContainer.x > rightEdgeInContainer.x) {
            break;
        }

        auto spr = CCSprite::create("slider-tick.png"_spr);
        spr->setOpacity(255);
        spr->setID(fmt::format("snap-tick-{}"_spr, numToString(value, 3)));

        if (posInContainer.x > rightGrooveEdgeInContainer.x) {
            spr->setColor({127, 255, 127});
            spr->setOpacity(127);
            auto posInExtendedGroove = m_extendedGroove->convertToNodeSpace(posWorld);
            spr->setPosition({posInExtendedGroove.x, m_extendedGroove->getContentHeight() / 2.f});
            m_extendedGroove->addChild(spr);
        }
        else {
            spr->setPosition({posInContainer.x, m_snapPointContainer->getContentHeight() / 2.f});
            m_snapPointContainer->addChild(spr);
        }

        i++;
        if (i > 100) break;
    }

    auto leftGrooveEdgeWorld = convertToWorldSpace({1.f, 0.f});
    auto leftGrooveEdgeInContainer = m_snapPointContainer->convertToNodeSpace(leftGrooveEdgeWorld);

    auto leftEdgeWorld = convertToWorldSpace({std::min(0.f, getGroove()->getContentWidth() * getPercent()) + 1.f, 0.f});
    auto leftEdgeInContainer = m_snapPointContainer->convertToNodeSpace(leftEdgeWorld);

    i = 1;
    while (true) {
        auto value = (i - m_scaleControl->m_lowerBound) / (m_scaleControl->m_upperBound - m_scaleControl->m_lowerBound);
        auto posWorld = convertToWorldSpace({getContentWidth() * value, 0.f});
        auto posInContainer = m_snapPointContainer->convertToNodeSpace(posWorld);

        for (int j = 1; j < m_snap; j++) {
            auto smallValue = (i - (j * (1.f / m_snap)) - m_scaleControl->m_lowerBound) / (m_scaleControl->m_upperBound - m_scaleControl->m_lowerBound);
            auto smallPosWorld = convertToWorldSpace({getContentWidth() * smallValue, 0.f});
            auto smallPosInContainer = m_snapPointContainer->convertToNodeSpace(smallPosWorld);

            if (smallPosInContainer.x < leftEdgeInContainer.x) {
                break;
            }

            auto spr = CCSprite::create("slider-tick-small.png"_spr);
            spr->setOpacity(105);
            spr->setID(fmt::format("snap-tick-small-{}"_spr, numToString(smallValue, 3)));

            if (smallPosInContainer.x < leftGrooveEdgeInContainer.x) {
                spr->setColor({255, 127, 127});
                spr->setOpacity(52);
                auto posInExtendedGroove = m_extendedGroove->convertToNodeSpace(smallPosWorld);
                spr->setPosition({posInExtendedGroove.x, m_extendedGroove->getContentHeight() / 2.f});
                m_extendedGroove->addChild(spr);
            }
            else {
                spr->setPosition({smallPosInContainer.x, m_snapPointContainer->getContentHeight() / 2.f});
                m_snapPointContainer->addChild(spr);
            }
        }

        if (posInContainer.x < leftEdgeInContainer.x) {
            break;
        }
        if (i == 1) {
            i--;
            continue;
        }

        auto spr = CCSprite::create("slider-tick.png"_spr);
        spr->setOpacity(255);
        spr->setID(fmt::format("snap-tick-{}"_spr, numToString(value, 3)));

        if (posInContainer.x < leftGrooveEdgeInContainer.x) {
            spr->setColor({255, 127, 127});
            spr->setOpacity(127);

            auto posInExtendedGroove = m_extendedGroove->convertToNodeSpace(posWorld);
            spr->setPosition({posInExtendedGroove.x, m_extendedGroove->getContentHeight() / 2.f});
            m_extendedGroove->addChild(spr);
        } 
        else {
            spr->setPosition({posInContainer.x, m_snapPointContainer->getContentHeight() / 2.f});
            m_snapPointContainer->addChild(spr);
        }

        i--;
        if (i < -100) break;
    }
}