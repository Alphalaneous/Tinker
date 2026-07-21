#include "NoTouchPasteStateFix.hpp"
#include <alphalaneous.alphas_geode_utils/include/ObjectModify.hpp>

bool NoTouchPasteStateFix::onToggled(bool state) {
    return true;
}

void NoTouchPasteStateFix::fixNoTouch(GameObject* object, CCArray* objects) {
    if (object && objects->count() == 0) {
        objects = CCArray::createWithObject(object);
    }

    for (auto obj : CCArrayExt<GameObject, false>(objects)) {
        if (obj->m_isNoTouch) {
            m_editorLayer->removeObjectFromSection(obj);

            obj->setType(obj->m_savedObjectType);
            obj->saveActiveColors();

            m_editorLayer->addToSection(obj);
        }
    }
}

void NTPSFEditorUI::onPasteState(cocos2d::CCObject* sender) {
    EditorUI::onPasteState(sender);
    NoTouchPasteStateFix::get()->fixNoTouch(m_selectedObject, m_selectedObjects);
}

class $nodeModify(PasteStatePopup) {

    void modify() {
        if (!NoTouchPasteStateFix::isEnabled()) return;

        addOnExitCallback([] {
            auto editorUI = EditorUI::get();
            if (!editorUI) return;
            NoTouchPasteStateFix::get()->fixNoTouch(editorUI->m_selectedObject, editorUI->m_selectedObjects);
        });
    }

};