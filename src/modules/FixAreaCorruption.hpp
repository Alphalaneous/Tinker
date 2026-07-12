#pragma once

#include "module/Module.hpp"
#include <Geode/modify/GJBaseGameLayer.hpp>

class $editorModule(FixAreaCorruption) {
};

class $modify(FACGJBaseGameLayer, GJBaseGameLayer) {
    $registerEditorHooks(FixAreaCorruption)

    struct Fields {
        bool m_skipAreaProcessing;
    };

    void loadUpToPosition(float position, int order, int channel);
    void processAreaEffects(gd::vector<EnterEffectInstance>* effects, GJAreaActionType type, float dt, bool visibleFrame);
};
