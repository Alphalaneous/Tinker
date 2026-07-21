#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $editorModule(FixSlopeHitboxRender) {
    bool onToggled(bool state) override;
};

class $modify(FSHREditorUI, EditorUI) {
    $registerEditorHooks(FixSlopeHitboxRender)

    struct Fields {
        bool m_checkSlopes;
    };

    void transformObjectCall(EditCommand command);
    bool isSpecialSnapObject(int id);
};