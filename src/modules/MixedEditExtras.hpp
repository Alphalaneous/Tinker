#pragma once

#include "module/Module.hpp"
#include <Geode/modify/SetupObjectOptionsPopup.hpp>

class $editorModule(MixedEditExtras) {
    bool onToggled(bool state);
};

class $modify(MEESetupObjectOptionsPopup, SetupObjectOptionsPopup) {
    $registerEditorHooks(MixedEditExtras)

    bool init(GameObject* object, cocos2d::CCArray* objects, SetGroupIDLayer* layer);
    void updateMixedToggles();

    template <class T>
    void checkMixedFor(std::string_view id, CCArray* objects, bool T::*member) {
        auto firstObj = typeinfo_cast<T*>(objects->firstObject());
        if (!firstObj) return;

        bool first = firstObj->*member;
        bool mixed = false;

        for (auto object : objects->asExt<GameObject>()) {
            auto objectCasted = typeinfo_cast<T*>(object);
            if (!objectCasted) return;
            if (objectCasted->*member != first) {
                mixed = true;
                break;
            }
        }

        auto toggler = static_cast<CCMenuItemToggler*>(m_buttonMenu->getChildByID(id));
        if (!toggler) return;

        float scaleOffset = 1.f;

        CCSprite* newSpr;
        if (mixed) {
            newSpr = CCSprite::create("GJ_button_05.png");
            newSpr->setOpacity(200);
            scaleOffset = 0.75f;
        }
        else {
            newSpr = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
        }

        toggler->m_offButton->setNormalImage(newSpr);

        newSpr->setAnchorPoint({0.5f, 0.5f});
        newSpr->setPosition(toggler->m_offButton->getContentSize() / 2);
        newSpr->setScale(0.7 * scaleOffset);
    }
};