#include "MixedEditExtras.hpp"

bool MixedEditExtras::onToggled(bool state) {
    return true;
}

bool MEESetupObjectOptionsPopup::init(GameObject* object, cocos2d::CCArray* objects, SetGroupIDLayer* layer) {
    if (!SetupObjectOptionsPopup::init(object, objects, layer)) return false;

    updateMixedToggles();

    for (auto node : m_buttonMenu->getChildrenExt()) {
        if (auto toggler = typeinfo_cast<CCMenuItemToggler*>(node)) {
            tinker::utils::hijackButton(toggler, [this] (auto orig, CCObject* sender) {
                orig(sender);
                updateMixedToggles();
            });
        }
    }

    return true;
}

void MEESetupObjectOptionsPopup::updateMixedToggles() {
    log::info("update mixed");
    CCArray* objects;
    if (m_gameObjects && m_gameObjects->count() > 0) {
        objects = m_gameObjects;
    }
    else if (m_gameObject) {
        objects = CCArray::createWithObject(m_gameObject);
    }
    if (!objects) return;

    checkMixedFor("dont-fade-toggle", objects, &GameObject::m_isDontFade);
    checkMixedFor("dont-enter-toggle", objects, &GameObject::m_isDontEnter);
    checkMixedFor("no-effects-toggle", objects, &GameObject::m_hasNoEffects);
    checkMixedFor("group-parent-toggle", objects, &GameObject::m_hasGroupParent);
    checkMixedFor("area-parent-toggle", objects, &GameObject::m_hasAreaParent);
    checkMixedFor("dont-boosty-toggle", objects, &GameObject::m_isDontBoostY);
    checkMixedFor("dont-boostx-toggle", objects, &GameObject::m_isDontBoostX);
    checkMixedFor("notouch-toggle", objects, &GameObject::m_isNoTouch);
    checkMixedFor("passable-toggle", objects, &GameObject::m_isPassable);
    checkMixedFor("hide-toggle", objects, &GameObject::m_isHide);
    checkMixedFor("nonstickx-toggle", objects, &GameObject::m_isNonStickX);
    checkMixedFor("extrasticky-toggle", objects, &GameObject::m_isExtraSticky);
    checkMixedFor("extended-collision-toggle", objects, &GameObject::m_hasExtendedCollision);
    checkMixedFor("iceblock-toggle", objects, &GameObject::m_isIceBlock);
    checkMixedFor("gripslope-toggle", objects, &GameObject::m_isGripSlope);
    checkMixedFor("noglow-toggle", objects, &GameObject::m_hasNoGlow);
    checkMixedFor("noparticle-toggle", objects, &GameObject::m_hasNoParticles);
    checkMixedFor("nonsticky-toggle", objects, &GameObject::m_isNonStickY);
    checkMixedFor("scalestick-toggle", objects, &GameObject::m_isScaleStick);
    checkMixedFor("no-audio-scale-toggle", objects, &GameObject::m_hasNoAudioScale);
    checkMixedFor("center-effect-toggle", objects, &EffectGameObject::m_hasCenterEffect);
    checkMixedFor("single-ptouch-toggle", objects, &EffectGameObject::m_isSinglePTouch);
    if (objects->count() == 1) {
        checkMixedFor("reverse-toggle", objects, &EffectGameObject::m_hasNoAudioScale);
    }
}