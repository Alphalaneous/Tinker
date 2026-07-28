#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

// also fixes smart template slope objects :3
class $editorModule(FixSlopeHitboxRender) {
    bool onToggled(bool state) override;
};

class $modify(FSHREditorUI, EditorUI) {
    $registerEditorHooks(FixSlopeHitboxRender)

    struct Fields {
        bool m_checkSlopes;
    };

    bool isSpecialSnapObject(int id);
};