#include "FixToggleTriggerVisibility.hpp"

void FTTVEffectGameObject::setOpacity(unsigned char opacity) {
    EffectGameObject::setOpacity(opacity);
    if (m_objectID == 1049) {
        auto spr = static_cast<CCSprite*>(getChildByTag(998));
        if (spr) spr->setOpacity(_realOpacity);
    }
}