#pragma once

#include "module/Module.hpp"
#include <Geode/modify/PlayerObject.hpp>

class $editorModule(FixIgnoreDamage) {
    bool onToggled(bool state) override;
};

class $modify(FIDPlayerObject, PlayerObject) {
    $registerEditorHooks(FixIgnoreDamage)

    void collidedWithSlopeInternal(float dt, GameObject* object, bool forced);
};