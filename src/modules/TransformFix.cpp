#include "TransformFix.hpp"

bool TransformFix::onToggled(bool state) {
    return true;
}

bool TFGJTransformControl::init() {
    if (!GJTransformControl::init()) return false;
    getMenu();
    return true;
}

CCMenu* TFGJTransformControl::getMenu() {
    auto fields = m_fields.self();
    if (fields->m_menu) {
        CCTouchDispatcher::get()->removeDelegate(fields->m_menu);
        return fields->m_menu;
    }

    if (!m_mainNode) return nullptr;

    CCNode* node = nullptr;
    for (auto child : m_mainNode->getChildrenExt()) {
        if (exact_cast<CCNode*>(child)) {
            node = child;
            break;
        }
    }
    if (!node) return nullptr;

    auto menu = node->getChildByType<CCMenu>(0);
    CCTouchDispatcher::get()->removeDelegate(menu);

    fields->m_menu = menu;
    return menu;
}

bool TFGJTransformControl::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    auto fields = m_fields.self();
    auto menu = getMenu();

    if (!menu) {
        fields->m_touchInMenu = false;
        return false;
    }

    auto menuRet = menu->ccTouchBegan(touch, event);
    fields->m_touchInMenu = menuRet;

    if (!menuRet) {
        return GJTransformControl::ccTouchBegan(touch, event);
    }
    
    return menuRet;
}

void TFGJTransformControl::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    auto fields = m_fields.self();
    if (fields->m_touchInMenu) {
        auto menu = getMenu();
        if (!menu) return;

        menu->ccTouchMoved(touch, event);
        return;
    }
    GJTransformControl::ccTouchMoved(touch, event);
}

void TFGJTransformControl::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    auto fields = m_fields.self();
    if (fields->m_touchInMenu) {
        fields->m_touchInMenu = false;

        auto menu = getMenu();
        if (!menu) return;

        menu->ccTouchEnded(touch, event);
        return;
    }
    GJTransformControl::ccTouchEnded(touch, event);
}

void TFGJTransformControl::ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {

    auto fields = m_fields.self();
    if (fields->m_touchInMenu) {
        fields->m_touchInMenu = false;

        auto menu = getMenu();
        if (!menu) return;

        menu->ccTouchCancelled(touch, event);
        return;
    }
    GJTransformControl::ccTouchCancelled(touch, event);
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

    if (fields->m_menu) {
        CCTouchDispatcher::get()->removeDelegate(fields->m_menu);
        return fields->m_menu;
    }
    
    auto menu = getChildByType<CCMenu>(0);
    if (!menu) return nullptr;

    CCTouchDispatcher::get()->removeDelegate(menu);

    fields->m_menu = menu;
    return menu;
}

bool TFGJScaleControl::init() {
    if (!GJScaleControl::init()) return false;
    getMenu();
    return true;
}

bool TFGJScaleControl::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    auto fields = m_fields.self();
    auto menu = getMenu();
    if (!menu) {
        fields->m_touchInMenu = false;
        return false;
    }

    auto menuRet = menu->ccTouchBegan(touch, event);
    fields->m_touchInMenu = menuRet;

    if (!menuRet) {
        return GJScaleControl::ccTouchBegan(touch, event);
    }
    return menuRet;
}

void TFGJScaleControl::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    auto fields = m_fields.self();
    if (fields->m_touchInMenu) {
        auto menu = getMenu();
        if (!menu) return;

        menu->ccTouchMoved(touch, event);
        return;
    }
    GJScaleControl::ccTouchMoved(touch, event);
}

void TFGJScaleControl::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    auto fields = m_fields.self();
    if (fields->m_touchInMenu) {
        fields->m_touchInMenu = false;

        auto menu = getMenu();
        if (!menu) return;

        menu->ccTouchEnded(touch, event);
        return;
    }
    GJScaleControl::ccTouchEnded(touch, event);
}

void TFGJScaleControl::ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    auto fields = m_fields.self();
    if (fields->m_touchInMenu) {
        fields->m_touchInMenu = false;

        auto menu = getMenu();
        if (!menu) return;

        menu->ccTouchCancelled(touch, event);
        return;
    }
    GJScaleControl::ccTouchCancelled(touch, event);
}