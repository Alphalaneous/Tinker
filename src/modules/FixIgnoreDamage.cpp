#include "FixIgnoreDamage.hpp"

bool FixIgnoreDamage::onToggled(bool state) {
    return true;
}

void FIDPlayerObject::collidedWithSlopeInternal(float dt, GameObject* object, bool forced) {
    if (m_isDart && m_ignoreDamage && m_stateDartSlide <= 0) return;
    PlayerObject::collidedWithSlopeInternal(dt, object, forced);
}
