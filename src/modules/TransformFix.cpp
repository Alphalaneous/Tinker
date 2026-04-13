#include "TransformFix.hpp"

bool TransformFix::onToggled(bool state) {
    return true;
}

CCMenu* TFGJTransformControl::getMenu() {
    auto fields = m_fields.self();

    if (fields->m_menu) return fields->m_menu;
    auto node = m_mainNode->getChildByType<CCNode>(0);
    if (!node) return nullptr;

    auto menu = node->getChildByType<CCMenu>(0);

    fields->m_menu = menu;
    return menu;
}

bool TFGJTransformControl::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    if (GJTransformControl::ccTouchBegan(touch, event)) return true;

    auto fields = m_fields.self();
    auto menu = getMenu();
    if (!menu) {
        fields->m_touchInMenu = false;
        return false;
    }

    auto menuRet = menu->ccTouchBegan(touch, event);
    fields->m_touchInMenu = menuRet;
    return menuRet;
}

void TFGJTransformControl::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    GJTransformControl::ccTouchMoved(touch, event);

    auto fields = m_fields.self();
    if (fields->m_touchInMenu) {
        auto menu = getMenu();
        if (!menu) return;

        menu->ccTouchMoved(touch, event);
    }
}

void TFGJTransformControl::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    GJTransformControl::ccTouchEnded(touch, event);

    auto fields = m_fields.self();
    if (fields->m_touchInMenu) {
        auto menu = getMenu();
        if (!menu) return;

        menu->ccTouchEnded(touch, event);
    }
    fields->m_touchInMenu = false;

}

void TFGJTransformControl::ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    GJTransformControl::ccTouchCancelled(touch, event);

    auto fields = m_fields.self();
    if (fields->m_touchInMenu) {
        auto menu = getMenu();
        if (!menu) return;

        menu->ccTouchCancelled(touch, event);
    }
    fields->m_touchInMenu = false;
}

void TFGJTransformControl::scaleButtons(float scale) {
    GJTransformControl::scaleButtons(scale);
    if (tinker::utils::getMod<"razoom.improved_transform_controls">()) return;
    if (!m_warpLockButton) return;

    auto spr = m_warpLockButton->getChildByIndex(0);
    if (!spr) return;

    m_warpLockButton->setContentSize(spr->getScaledContentSize());
    spr->setPosition(m_warpLockButton->getContentSize()/2);
}

CCMenu* TFGJScaleControl::getMenu() {
    auto fields = m_fields.self();

    if (fields->m_menu) return fields->m_menu;
    auto menu = getChildByType<CCMenu>(0);
    if (!menu) return nullptr;

    fields->m_menu = menu;
    return menu;
}

bool TFGJScaleControl::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    if (GJScaleControl::ccTouchBegan(touch, event)) return true;
    auto fields = m_fields.self();
    auto menu = getMenu();
    if (!menu) {
        fields->m_touchInMenu = false;
        return false;
    }

    auto ret = menu->ccTouchBegan(touch, event);
    fields->m_touchInMenu = ret;
    return ret;
}

void TFGJScaleControl::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    GJScaleControl::ccTouchMoved(touch, event);
    auto fields = m_fields.self();
    if (fields->m_touchInMenu) {
        auto menu = getMenu();
        if (!menu) return;

        menu->ccTouchMoved(touch, event);
    }
}

void TFGJScaleControl::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    GJScaleControl::ccTouchEnded(touch, event);
    auto fields = m_fields.self();
    if (fields->m_touchInMenu) {
        auto menu = getMenu();
        if (!menu) return;

        menu->ccTouchEnded(touch, event);
    }
    fields->m_touchInMenu = false;

}

void TFGJScaleControl::ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    GJScaleControl::ccTouchCancelled(touch, event);
    auto fields = m_fields.self();
    if (fields->m_touchInMenu) {
        auto menu = getMenu();
        if (!menu) return;

        menu->ccTouchCancelled(touch, event);
    }
    fields->m_touchInMenu = false;
}