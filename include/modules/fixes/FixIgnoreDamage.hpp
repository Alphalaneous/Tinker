#pragma once

#include "module/Module.hpp"
#include <Geode/modify/PlayerObject.hpp>

class $module(FixIgnoreDamage) {
    bool onToggled(bool state);
};

class $modify(FIDPlayerObject, PlayerObject) {
    $registerHooks(FixIgnoreDamage)

    void collidedWithSlopeInternal(float dt, GameObject* object, bool forced);
};