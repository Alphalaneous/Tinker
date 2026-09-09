#include <Geode/Geode.hpp>
#include <Geode/modify/GJRotationControl.hpp>
#include <alphalaneous.alphas_geode_utils/include/ObjectModify.hpp>

using namespace geode::prelude;

// fixes made to BetterEdit to get people to not report BetterEdit bugs as Tinker bugs

class $nodeModify(BEFixPasteStatePopup, PasteStatePopup) {
    void modify() {
        addOnExitCallback([] {
            auto editorUI = EditorUI::get();
            if (!editorUI) return;

            auto editorButtonsMenu = editorUI->getChildByID("editor-buttons-menu");
            if (!editorButtonsMenu) return;

            auto pasteStateButton = static_cast<CCMenuItemSpriteExtra*>(editorButtonsMenu->getChildByID("paste-state-button"));
            if (!pasteStateButton) return;

            pasteStateButton->setOpacity(255);
        });
    }
};

class $modify(BEFixGJRotationControl, GJRotationControl) {
    void finishTouch() {
        GJRotationControl::finishTouch();
        for (auto child : getChildrenExt()) {
            if (auto textInput = typeinfo_cast<geode::TextInput*>(child)) {
                textInput->defocus();
            }
        }
    }
};