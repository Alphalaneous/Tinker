#pragma once

#include "module/Module.hpp"
#include <Geode/modify/SetupObjectOptionsPopup.hpp>

class $module(MixedEditExtras) {
    bool onToggled(bool state);
};

class $modify(MEESetupObjectOptionsPopup, SetupObjectOptionsPopup) {
    $registerHooks(MixedEditExtras)

    bool init(GameObject* object, cocos2d::CCArray* objects);
    void updateMixedToggles();

    template <class T>
    void checkMixedFor(std::string_view id, cocos2d::CCArray* objects, bool T::*member) {
        auto firstObj = geode::cast::typeinfo_cast<T*>(objects->firstObject());
        if (!firstObj) return;

        bool first = firstObj->*member;
        bool mixed = false;

        for (auto object : objects->asExt<GameObject>()) {
            auto objectCasted = geode::cast::typeinfo_cast<T*>(object);
            if (!objectCasted) return;
            if (objectCasted->*member != first) {
                mixed = true;
                break;
            }
        }

        auto toggler = static_cast<CCMenuItemToggler*>(m_buttonMenu->getChildByID(id));
        if (!toggler) return;

        float scaleOffset = 1.f;

        cocos2d::CCSprite* newSpr;
        if (mixed) {
            newSpr = cocos2d::CCSprite::create("GJ_button_05.png");
            newSpr->setOpacity(200);
            scaleOffset = 0.75f;
        }
        else {
            newSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
        }

        toggler->m_offButton->setNormalImage(newSpr);

        newSpr->setAnchorPoint({0.5f, 0.5f});
        newSpr->setPosition(toggler->m_offButton->getContentSize() / 2.f);
        newSpr->setScale(0.7f * scaleOffset);
    }
};