#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $editorModule(QuickExtras) {
    CCMenuItemSpriteExtra* m_editExtrasBtn;

    void onEditor() override;
    void onEditExtras();
};

class $modify(QEEditorUI, EditorUI) {
    $registerEditorHooks(QuickExtras)

    void editObjectSpecial(int type);

    bool _isAllowedObjectID(int id);
    bool isDisallowedObjectID(int id);
    bool _checkMultiSelection();
    bool _editButton2Usable();

    bool isSpecialEdit(GameObject* obj);
};