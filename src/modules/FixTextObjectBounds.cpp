#include "FixTextObjectBounds.hpp"
#include "ReferenceImage.hpp"

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

void FTOBTextGameObject::customObjectSetup(gd::vector<gd::string>& p0, gd::vector<void*>& p1) {
    TextGameObject::customObjectSetup(p0, p1);
    fixBounds();
}

void FTOBTextGameObject::updateTextObject(gd::string p0, bool p1) {
    TextGameObject::updateTextObject(p0, p1);
    fixBounds();
}