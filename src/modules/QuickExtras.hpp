#pragma once

#include "../Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $editorModule(QuickExtras) {
    CCMenuItemSpriteExtra* m_editExtrasBtn;

    void onEditor() override;
    void onUpdateButtons() override;
    void onEditExtras();
};

class $modify(QEEditorUI, EditorUI) {
    $registerEditorHooks(QuickExtras)

    void showUI(bool show);
    bool _isAllowedObjectID(int id);
    bool isDisallowedObjectID(int id);
    bool _checkMultiSelection();
    bool _editButton2Usable();

    bool isSpecialEdit(GameObject* obj);
};