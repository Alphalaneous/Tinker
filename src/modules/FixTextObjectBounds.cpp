#include "FixTextObjectBounds.hpp"
#include "ReferenceImage.hpp"

bool FixTextObjectBounds::onToggled(bool state) {
    return true;
}

void FTOBTextGameObject::fixBounds() {
    if (ReferenceImage::isEnabled()) {
        auto ref = static_cast<RITextGameObject*>(static_cast<TextGameObject*>(this));
        if (ref->isReferenceImage()) return;
    }
    
    m_width = getContentWidth();
    m_height = getContentHeight();
    updateOrientedBox();

    auto editorUI = EditorUI::get();
    if (!editorUI) return;

    editorUI->updateTransformControl();
}

void FTOBTextGameObject::customObjectSetup(gd::vector<gd::string>& values, gd::vector<void*>& exists) {
    TextGameObject::customObjectSetup(values, exists);
    fixBounds();
}

void FTOBTextGameObject::updateTextObject(gd::string text, bool defaultFont) {
    TextGameObject::updateTextObject(text, defaultFont);
    fixBounds();
}