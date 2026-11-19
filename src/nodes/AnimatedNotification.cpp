#include "nodes/AnimatedNotification.hpp"
#include <Geode/utils/ZStringView.hpp>
#include "actions/CCValueTo.hpp"
#include "modules/UIScaling.hpp"

using namespace geode::prelude;

namespace tinker::ui {

AnimatedNotification* AnimatedNotification::create(ZStringView text) {
    auto ret = new AnimatedNotification();
    if (ret->init(text)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool AnimatedNotification::init(ZStringView text) {
    if (!CCNodeRGBA::init()) return false;

    setZOrder(9999);
    setAnchorPoint({0.5f, 1.f});

    m_container = geode::NineSlice::create("simple-popup-square.png"_spr);
    m_container->setContentSize(getContentSize());
    m_container->setOpacity(100);
    m_container->setID("background"_spr);

    float padding = 6.f;

    m_label = geode::Label::create(text, "bigFont.fnt");
    m_label->setAnchorPoint({0.5f, 0.5f});
    m_label->setScale(0.3f);
    m_label->setID("time-label"_spr);
    m_label->setOpacity(0);

    m_container->addChild(m_label);

    setContentSize({padding + m_label->getScaledContentWidth() + padding + 2.f, 30.f});

    m_label->setPositionY(getContentHeight() / 2.f);

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

void AnimatedNotification::show() {
    removeFromParent();
    auto editor = EditorUI::get();

    editor->addChild(this);

    setScale(UIScaling::getScale());
    setPosition({editor->m_positionSlider->getPositionX(), editor->m_positionSlider->boundingBox().getMinY() - 3.f * UIScaling::getScale()});
            
    m_container->setContentWidth(20.f);
    m_container->stopAllActions();

    m_label->runAction(CCFadeTo::create(0.25f, 255));
    m_render->runAction(CCFadeTo::create(0.25f, 255));

    auto contentWidthAction = CCEaseOut::create(CCValueTo<float>::create(0.1f, m_container->getContentWidth(), getContentWidth(), [this] (float t, float start, float end, float& width) {
        width = start + (end - start) * t;
        m_label->setPositionX(width / 2.f);
        m_container->setContentWidth(width);
    }), 1.2f);

    m_container->runAction(CCSequence::create(
        contentWidthAction, 
        CCDelayTime::create(1.f),
        CallFuncExt::create([this] {
            m_render->runAction(CCSequence::create(
                CCFadeTo::create(0.2f, 0),
                CallFuncExt::create([this] {
                    removeFromParent();
                }),
                nullptr
            ));
        }),
        nullptr
    ));

}

}
