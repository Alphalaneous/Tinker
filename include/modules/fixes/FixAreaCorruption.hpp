#pragma once

#include "module/Module.hpp"
#include <Geode/modify/GJBaseGameLayer.hpp>

class $module(FixAreaCorruption) {
};

class $modify(FACGJBaseGameLayer, GJBaseGameLayer) {
    $registerHooks(FixAreaCorruption)

    struct Fields {
        bool m_skipAreaProcessing;
    };

    void loadUpToPosition(float position, int order, int channel);
    void processAreaEffects(gd::vector<EnterEffectInstance>* effects, GJAreaActionType type, float dt, bool visibleFrame);
};
