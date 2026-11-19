#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

// also fixes smart template slope objects :3
class $module(FixSlopeHitboxRender) {
    bool onToggled(bool state);
};

class $modify(FSHEditorUI, EditorUI) {
    $registerHooks(FixSlopeHitboxRender)

    void refreshObject(GameObject* object);
    void transformObject(GameObject* object, EditCommand command, bool noOffset);
    GameObject* onCreateObject(int id);
};