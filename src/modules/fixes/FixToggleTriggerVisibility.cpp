#include "modules/fixes/FixToggleTriggerVisibility.hpp"
#include "utils/Constants.hpp"

void FTTVEffectGameObject::setOpacity(unsigned char opacity) {
    EffectGameObject::setOpacity(opacity);
    if (m_objectID == tinker::constants::objects::ToggleTrigger) {
        auto spr = static_cast<CCSprite*>(getChildByTag(998));
        if (spr) spr->setOpacity(_realOpacity);
    }
}