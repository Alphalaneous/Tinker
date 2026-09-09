#pragma once

#include "module/Module.hpp"
#include <Geode/modify/GJTransformControl.hpp>

class $module(TransformFix) {
    bool onToggled(bool state);
};

class $modify(TFGJTransformControl, GJTransformControl) {
    $registerHooks(TransformFix);

    void scaleButtons(float scale);
};