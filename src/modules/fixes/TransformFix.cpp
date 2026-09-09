#include "modules/fixes/TransformFix.hpp"

bool TransformFix::onToggled(bool state) {
    return true;
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