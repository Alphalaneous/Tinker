#include "NoTouchPasteStateFix.hpp"
#include <alphalaneous.alphas_geode_utils/include/ObjectModify.hpp>

bool NoTouchPasteStateFix::onToggled(bool state) {
    return true;
}

void NoTouchPasteStateFix::fixNoTouch(GameObject* object, CCArray* objects) {
    if (object && objects->count() == 0) {
        objects = CCArray::createWithObject(object);
    }

    auto lel = LevelEditorLayer::get();

    for (auto obj : CCArrayExt<GameObject, false>(objects)) {
        if (obj->m_isNoTouch) {
            lel->removeObjectFromSection(obj);

            obj->setType(obj->m_savedObjectType);
            obj->saveActiveColors();

            lel->addToSection(obj);
        }
    }
}

void NTPSFEditorUI::onPasteState(cocos2d::CCObject* sender) {
    EditorUI::onPasteState(sender);
    NoTouchPasteStateFix::get()->fixNoTouch(m_selectedObject, m_selectedObjects);
}

class $nodeModify(PasteStatePopup) {

    void modify() {
        addOnExitCallback([] {
            auto editorUI = EditorUI::get();
            if (!editorUI) return;
            NoTouchPasteStateFix::get()->fixNoTouch(editorUI->m_selectedObject, editorUI->m_selectedObjects);
        });
    }

};