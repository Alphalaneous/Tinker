#include "nodes/ShadowLabel.hpp"
#include "utils/UI.hpp"

using namespace geode::prelude;

namespace tinker::ui {

ShadowLabel* ShadowLabel::create(ZStringView text, ZStringView font, float shadowDistance) {
    auto ret = new ShadowLabel();
    if (ret->init(text, font, shadowDistance)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool ShadowLabel::init(ZStringView text, ZStringView font, float shadowDistance) {
    if (!CCNodeRGBA::init()) return false;

    setCascadeColorEnabled(true);
    setCascadeOpacityEnabled(true);

    m_shadowDistance = shadowDistance;
    m_container = CCNode::create();

    m_mainLabel = geode::Label::create(text, font);
    m_shadowLabel = geode::Label::create(text, font);

    m_mainLabel->setAnchorPoint({0.f, 0.f});
    m_shadowLabel->setAnchorPoint({0.f, 0.f});
    m_shadowLabel->setPosition({shadowDistance, -shadowDistance});

    m_shadowLabel->setZOrder(-1);
    m_shadowLabel->setColor({0, 0, 0});
    m_shadowLabel->setOpacity(127);

    m_container->addChild(m_mainLabel);
    m_container->addChild(m_shadowLabel);

    auto bounds = tinker::utils::ui::getRealBounds(m_container);
    m_container->setContentSize(bounds.size);

    setContentSize(m_container->getContentSize());

    m_render = alpha::ui::RenderNode::create(m_container, false);
    addChild(m_render);

    return true;
}

void ShadowLabel::validate() {
    m_mainLabel->validate();
    m_shadowLabel->validate();
}

void ShadowLabel::setLimitLabelWidth(float width, float maxScale, float minScale) {
    m_mainLabel->setLimitLabelWidth(width, maxScale, minScale);
    m_shadowLabel->setLimitLabelWidth(width, maxScale, minScale);

    m_shadowLabel->setPosition({m_shadowDistance * m_mainLabel->getScale(), -m_shadowDistance * m_mainLabel->getScale()});

    m_container->setContentSize(m_mainLabel->getScaledContentSize());
    setContentSize(m_container->getContentSize());

    m_render->removeFromParent();

    m_render = alpha::ui::RenderNode::create(m_container, false);
    addChild(m_render);
}

float ShadowLabel::getLabelScale() {
    return m_mainLabel->getScale();
}

void ShadowLabel::setText(ZStringView text) {
    m_mainLabel->setText(text);
    m_shadowLabel->setText(text);

    m_shadowLabel->setPosition({m_shadowDistance * m_mainLabel->getScale(), -m_shadowDistance * m_mainLabel->getScale()});

    auto bounds = tinker::utils::ui::getRealBounds(m_container);

    m_container->setContentSize(m_mainLabel->getScaledContentSize());
    setContentSize(m_container->getContentSize());

    m_render->removeFromParent();

    m_render = alpha::ui::RenderNode::create(m_container, false);
    addChild(m_render);
}

}