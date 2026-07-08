#include "CopyObjectString.hpp"

bool CopyObjectString::onToggled(bool state) {
    return true;
}

bool CopyObjectString::isObjectString(ZStringView str) {
    auto it = str.begin();
    for (; it != str.end() && std::isdigit(static_cast<unsigned char>(*it)); ++it);
    return it != str.begin() && it != str.end() && *it == ',';
}

void COSEditorUI::doCopyObjects(bool withColor) {
    EditorUI::doCopyObjects(withColor);
    if (GameManager::get()->m_editorClipboard.empty()) return;

    clipboard::write(GameManager::get()->m_editorClipboard);
}

void COSEditorUI::doPasteObjects(bool withColor) {
    auto clipboard = clipboard::read();
    if (!CopyObjectString::isObjectString(clipboard)) {
        EditorUI::doPasteObjects(withColor);
    }
    else {
        GameManager::get()->m_editorClipboard = clipboard;
        EditorUI::doPasteObjects(withColor);
        Notification::create("Pasted Objects from Clipboard", NotificationIcon::Info)->show();
    }
}