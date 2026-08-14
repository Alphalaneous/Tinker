#include "settings/SupportButton.hpp"
#include <alphalaneous.alphas-ui-pack/include/Utils.hpp>

namespace tinker::ui {

SupportButton* SupportButton::create(float width) {
    auto ret = new SupportButton();
    if (ret->init(width)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool SupportButton::init(float width) {
    if (!CCLayer::init()) return false;

    setTouchEnabled(true);
    ignoreAnchorPointForPosition(false);
    setAnchorPoint({0.f, 0.f});

    m_banner = CCSprite::create("support-banner.png"_spr);
    m_banner->setScale(width / m_banner->getContentWidth());
    m_banner->setID("support-banner"_spr);
    m_banner->setCascadeColorEnabled(true);
    m_banner->setAnchorPoint({0.f, 0.f});

    #ifndef GEODE_IS_ANDROID32
    auto alphaSmile = CCSprite::create("alpha-smile.gif"_spr);
    alphaSmile->setAnchorPoint({1.f, 0.f});
    alphaSmile->setPositionX(m_banner->getContentWidth());
    alphaSmile->setScale(m_banner->getContentHeight() / alphaSmile->getContentHeight());
    alphaSmile->setID("alpha-smile-:3"_spr);

    m_banner->addChild(alphaSmile);
    #endif

    addChild(m_banner);
    setContentSize(m_banner->getContentSize());
    return true;
}

void SupportButton::registerWithTouchDispatcher() {
    CCTouchDispatcher::get()->addTargetedDelegate(this, -512, true);
}

bool SupportButton::ccTouchBegan(CCTouch* touch, CCEvent* event) {
    if (!nodeIsVisible(this) || !alpha::utils::isPointInsideNode(this, touch->getLocation())) return false;
    m_banner->setColor({200, 200, 200});
    return true;
}

void SupportButton::ccTouchMoved(CCTouch* touch, CCEvent* event) {
    bool inNode = alpha::utils::isPointInsideNode(this, touch->getLocation());

    m_banner->setColor(inNode ? ccColor3B{200, 200, 200} : ccColor3B{255, 255, 255});
}

void SupportButton::ccTouchEnded(CCTouch* touch, CCEvent* event) {
    m_banner->setColor({255, 255, 255});
    if (alpha::utils::isPointInsideNode(this, touch->getLocation())) {
        web::openLinkInBrowser("https://ko-fi.com/alphalaneous");
    }
}

void SupportButton::ccTouchCancelled(CCTouch* touch, CCEvent* event) {
    m_banner->setColor({255, 255, 255});
}

}