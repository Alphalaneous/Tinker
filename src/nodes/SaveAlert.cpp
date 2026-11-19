#include "nodes/SaveAlert.hpp"
#include "actions/CCValueTo.hpp"
#include "nodes/GIF.hpp"

using namespace geode::prelude;

namespace tinker::ui {

SaveAlert* SaveAlert::create() {
    auto ret = new SaveAlert();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool SaveAlert::init() {
    if (!CCNodeRGBA::init()) return false;

    setContentSize({30.f, 30.f});
    setZOrder(9999);

    m_container = geode::NineSlice::create("simple-popup-square.png"_spr);
    m_container->setContentSize(getContentSize());
    m_container->setOpacity(100);
    m_container->setID("background"_spr);

    float padding = 6.f;

    m_label = geode::Label::create("bigFont.fnt");
    m_label->setAnchorPoint({0.f, 0.5f});
    m_label->setScale(0.3f);
    m_label->setID("time-label"_spr);

    m_saveIcon = tinker::ui::GIF::create("save-anim");
    m_saveIcon->setAnchorPoint({0.f, 0.5f});
    m_saveIcon->setScale((getContentHeight() - padding * 2.f) / m_saveIcon->getContentHeight());
    m_saveIcon->setPosition({padding, getContentHeight() / 2.f});

    m_label->setPosition({m_saveIcon->boundingBox().getMaxX() + padding, getContentHeight() / 2.f});

    m_container->addChild(m_saveIcon);
    m_container->addChild(m_label);

    m_render = alpha::ui::RenderNode::create(m_container, false);

    m_render->setPosition(getContentSize() / 2.f);
    m_render->setOpacity(0);
    m_render->setID("content-render"_spr);

    m_render->setContentSize(getContentSize());
    m_container->setContentSize(getContentSize());
    m_container->setPosition(getContentSize() / 2.f);

    addChild(m_render);

    return true;
}

void SaveAlert::setAlertColor(const ccColor3B& color) {
    m_label->setColor(color);
    m_container->setColor(color);
}

void SaveAlert::setText(ZStringView text) {
    if (m_text == text) return;

    m_text = text;
    m_label->setText(text);
    float padding = 6.f;

    setContentSize({padding + m_saveIcon->getScaledContentWidth() + padding + m_label->getScaledContentWidth() + padding + 2.f, 30.f});

    m_container->stopAllActions();
    auto contentWidthAction = CCEaseOut::create(CCValueTo<float>::create(0.1f, m_container->getContentWidth(), getContentWidth(), [this] (float t, float start, float end, float& width) {
        width = start + (end - start) * t;
        m_container->setContentWidth(width);
    }), 1.2f);

    m_container->runAction(contentWidthAction);

    m_container->setPosition(getContentSize() / 2.f);

    m_render->setContentSize(getContentSize());
    m_render->setPosition(getContentSize() / 2.f);
}

void SaveAlert::show() {
    removeFromParent();
    EditorUI::get()->addChild(this);
    m_render->runAction(CCFadeTo::create(0.25f, 255));
}

void SaveAlert::hide() {
    m_render->runAction(CCSequence::create(
        CCDelayTime::create(1.f), 
        CCFadeTo::create(0.2f, 0), 
        CallFuncExt::create([this] {
            // fixes a weird leak
            m_saveIcon->removeFromParent();
            removeFromParent();
        }),
        nullptr
    ));
}

}