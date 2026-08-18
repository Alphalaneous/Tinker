#include "nodes/ShadowLabel.hpp"
#include "utils/Utils.hpp"

namespace tinker::ui {

ShadowLabel* ShadowLabel::create(ZStringView text, ZStringView font) {
    auto ret = new ShadowLabel();
    if (ret->init(text, font)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool ShadowLabel::init(ZStringView text, ZStringView font) {
    if (!CCNodeRGBA::init()) return false;

    setCascadeColorEnabled(true);
    setCascadeOpacityEnabled(true);

    m_container = CCNode::create();

    m_mainLabel = geode::Label::create(text, font);
    m_shadowLabel = geode::Label::create(text, font);

    m_mainLabel->setAnchorPoint({0.f, 0.f});
    m_shadowLabel->setAnchorPoint({0.f, 0.f});
    m_shadowLabel->setPosition({1.f, -1.f});

    m_shadowLabel->setZOrder(-1);
    m_shadowLabel->setColor({0, 0, 0});
    m_shadowLabel->setOpacity(127);

    m_container->addChild(m_mainLabel);
    m_container->addChild(m_shadowLabel);

    auto bounds = utils::getRealBounds(m_container);
    m_container->setContentSize(bounds.size);

    setContentSize(m_container->getContentSize());

    m_render = alpha::ui::RenderNode::create(m_container, true);
    addChild(m_render);

    return true;
}

void ShadowLabel::setText(ZStringView text) {
    m_mainLabel->setText(text);
    m_shadowLabel->setText(text);

    auto bounds = utils::getRealBounds(m_container);

    m_container->setContentSize(bounds.size);
    setContentSize(m_container->getContentSize());

    m_render->removeFromParent();

    m_render = alpha::ui::RenderNode::create(m_container, true);
    addChild(m_render);
}

}