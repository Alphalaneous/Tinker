#include "modules/fixes/FixAreaCorruption.hpp"

void FACGJBaseGameLayer::loadUpToPosition(float position, int order, int channel) {
    auto fields = m_fields.self();

    fields->m_skipAreaProcessing = true;
    GJBaseGameLayer::loadUpToPosition(position, order, channel);
    fields->m_skipAreaProcessing = false;
}

void FACGJBaseGameLayer::processAreaEffects(gd::vector<EnterEffectInstance>* effects, GJAreaActionType type, float dt, bool visibleFrame) {
    auto fields = m_fields.self();
    if (fields->m_skipAreaProcessing) return;
    GJBaseGameLayer::processAreaEffects(effects, type, dt, visibleFrame);
}
