#include "PlaytestConfirmation.hpp"

bool PlaytestConfirmation::onToggled(bool state) {
    return true;
}

void PCEditorUI::onPlaytest(CCObject* sender) {
    if (m_editorLayer->m_playbackMode != PlaybackMode::Not) {
        return EditorUI::onPlaytest(sender);
    }
    if (auto node = typeinfo_cast<CCNode*>(sender)) {
        if (node->getUserFlag("start-pos-switcher"_spr)) {
            return EditorUI::onPlaytest(sender);
        }
    }
    auto fields = m_fields.self();
    if (fields->m_doPlaytest) {
        EditorUI::onPlaytest(sender);
        fields->m_doPlaytest = false;
        return;
    }
    createQuickPopup("Playtest", "Are you sure you want to playtest?", "No", "Yes", [this, sender, fields] (auto alert, bool pressed) {
        if (pressed) {
            fields->m_doPlaytest = true;
            EditorUI::onPlaytest(sender);
        }
    });
}