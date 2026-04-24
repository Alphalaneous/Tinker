#pragma once

#include "../../Module.hpp"
#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>

class $editorModule(AdvancedSelectAll) {
    void onEditor() override;
};

class $modify(ASAEditorUI, EditorUI) {
    $registerEditorHooks(AdvancedSelectAll)

    struct Fields {
        Ref<CCMenuItemSpriteExtra> m_selectAllOfObjectButton;
        Ref<CCMenuItemSpriteExtra> m_selectAllOfBaseColorButton;
        Ref<CCMenuItemSpriteExtra> m_selectAllOfDetailColorButton;
    };

    void updateButtons();
    void onSelectAllOfObjectButton(CCObject* sender);
    void onSelectAllOfBaseColorButton(CCObject* sender);
    void onSelectAllOfDetailColorButton(CCObject* sender);

    void setButtonColor(CCMenuItemSpriteExtra* btn, const ccColor3B& color);
};