#include "ThatPasteButton.hpp"
#include "../Utils.hpp"

bool ThatPasteButton::onToggled(bool state) {
    if (state) {
        onEditor();
    }
    return true;
}

bool ThatPasteButton::onSettingChanged(std::string_view key, const matjson::Value& value) {
    return true;
}

bool ThatPasteButton::isGroupDefault(GameObject* object) {
    if (object->m_groups) {
        return object->m_groups->empty();
    }
    return true;
}

bool ThatPasteButton::areObjectGroupsDefault() {
    if (m_editorUI->m_selectedObject) {
        return isGroupDefault(m_editorUI->m_selectedObject);
    }
    else if (m_editorUI->m_selectedObjects) {
        for (auto obj : m_editorUI->m_selectedObjects->asExt<GameObject>()) {
            bool isDefault = isGroupDefault(obj);
            if (!isDefault) return false;
        }
    }

    return true;
}

bool ThatPasteButton::isColorDefault(GameObject* object) {
    bool defaultBase = true;
    if (object->m_baseColor) {
        defaultBase = object->m_baseColor->m_colorID == 0 || object->m_baseColor->m_colorID == object->m_baseColor->m_defaultColorID;
    }

    bool defaultDetail = true;
    if (object->m_detailColor) {
        defaultDetail = object->m_detailColor->m_colorID == 0 || object->m_detailColor->m_colorID == object->m_detailColor->m_defaultColorID;
    }

    if (defaultBase && defaultDetail) {
        return true;
    }
    return false;
}

bool ThatPasteButton::areObjectColorsDefault() {
    if (m_editorUI->m_selectedObject) {
        return isColorDefault(m_editorUI->m_selectedObject);
    }
    else if (m_editorUI->m_selectedObjects) {
        for (auto obj : m_editorUI->m_selectedObjects->asExt<GameObject>()) {
            bool isDefault = isColorDefault(obj);
            if (!isDefault) return false;
        }
    }

    return true;
}

void ThatPasteButton::onEditor() {
    auto buttons = m_editorUI->getChildByID("editor-buttons-menu");
    if (!buttons) return;

    if (auto pasteStateButton = typeinfo_cast<CCMenuItemSpriteExtra*>(buttons->getChildByID("paste-state-button"))) {
        if (pasteStateButton->getUserObject("hijack"_spr)) return;

        tinker::utils::hijackButton(pasteStateButton, [this, pasteStateButton] (auto orig, auto sender) {
            if (!ThatPasteButton::isEnabled() || !getSetting<bool, "toggle-paste-state">()) return orig(sender);

            int showCount = getSetting<int, "object-requirement">();
            bool show = (m_editorUI->m_selectedObjects && m_editorUI->m_selectedObjects->count() >= showCount) || (showCount == 1 && m_editorUI->m_selectedObject);

            if (show && pasteStateButton->getOpacity() == 255) {
                if (getSetting<bool, "dont-show-if-default">() && areObjectGroupsDefault()) {
                    orig(sender);
                    return;
                }
                createQuickPopup("Paste State?", "Pasting state is <cr>dangerous</c>! Are you sure?", "Cancel", "Yes", [this, orig, sender] (FLAlertLayer*, bool yes) {
                    if (yes) orig(sender);
                });
            }
            else orig(sender);
        });
    }

    if (auto pasteColorButton = typeinfo_cast<CCMenuItemSpriteExtra*>(buttons->getChildByID("paste-color-button"))) {
        if (pasteColorButton->getUserObject("hijack"_spr)) return;

        tinker::utils::hijackButton(pasteColorButton, [this, pasteColorButton] (auto orig, auto sender) {
            if (!ThatPasteButton::isEnabled() || !getSetting<bool, "toggle-paste-color">()) return orig(sender);

            int showCount = getSetting<int, "object-requirement">();
            bool show = (m_editorUI->m_selectedObjects && m_editorUI->m_selectedObjects->count() >= showCount) || (showCount == 1 && m_editorUI->m_selectedObject);
            
            if (show && pasteColorButton->getOpacity() == 255) {
                if (getSetting<bool, "dont-show-if-default">() && areObjectColorsDefault()) {
                    orig(sender);
                    return;
                }
                createQuickPopup("Paste Color?", "Pasting color is <cr>dangerous</c>! Are you sure?", "Cancel", "Yes", [this, orig, sender] (FLAlertLayer*, bool yes) {
                    if (yes) orig(sender);
                });
            } 
            else orig(sender);
        });
    }
}

void ThatPasteButton::onSetGroupIDLayer(SetGroupIDLayer* setGroupIDLayer, GameObject* obj, CCArray* objs) {
    auto actions = setGroupIDLayer->m_mainLayer->getChildByID("actions-menu");
    if (!actions) return;

    auto pasteButton = typeinfo_cast<CCMenuItemSpriteExtra*>(actions->getChildByID("paste-button"));
    if (!pasteButton) return;

    if (pasteButton->getUserObject("hijack"_spr)) return;

    tinker::utils::hijackButton(pasteButton, [this] (auto orig, auto sender) {
        if (!ThatPasteButton::isEnabled() || !getSetting<bool, "toggle-paste-state-group">()) return orig(sender);

        int showCount = getSetting<int, "object-requirement">();
        bool show = (m_editorUI->m_selectedObjects && m_editorUI->m_selectedObjects->count() >= showCount) || (showCount == 1 && m_editorUI->m_selectedObject);
            
        if (show) {
            if (getSetting<bool, "dont-show-if-default">() && areObjectGroupsDefault()) {
                orig(sender);
                return;
            }
            createQuickPopup("Paste State?", "Pasting state is <cr>dangerous</c>! Are you sure?", "Cancel", "Yes", [this, orig, sender] (FLAlertLayer*, bool yes) {
                if (yes) orig(sender);
            });
        } 
        else orig(sender);
    });
}
