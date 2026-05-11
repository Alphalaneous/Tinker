#pragma once

#include <Geode/loader/Event.hpp>

namespace tinker::api::ui_scaling {
    struct UIScaleUpdated final : geode::Event<UIScaleUpdated, bool(float scale, bool scaleToolbars, bool topAlign)> {
        using Event::Event;
    };
    
    struct PauseUIScaleUpdated final : geode::Event<PauseUIScaleUpdated, bool(float scale)> {
        using Event::Event;
    };
};