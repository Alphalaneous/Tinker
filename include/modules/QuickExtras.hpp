#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $module(QuickExtras) {
    CCMenuItemSpriteExtra* m_editExtrasBtn;

    void onEditor();
    void onEditExtras();
};

class $modify(QEEditorUI, EditorUI) {
    $registerHooks(QuickExtras)

    void editObjectSpecial(int type);

    bool _isAllowedObjectID(int id);
    bool isDisallowedObjectID(int id);
    bool _checkMultiSelection();
    bool _editButton2Usable();

    bool isSpecialEdit(GameObject* obj);
};