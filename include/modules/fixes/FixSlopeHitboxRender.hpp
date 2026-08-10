#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

// also fixes smart template slope objects :3
class $module(FixSlopeHitboxRender) {
    bool onToggled(bool state);
};

class $modify(FSHREditorUI, EditorUI) {
    $registerHooks(FixSlopeHitboxRender)

    struct Fields {
        bool m_checkSlopes;
    };

    bool isSpecialSnapObject(int id);
};