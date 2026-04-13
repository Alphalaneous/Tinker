#include "SingleDeselect.hpp"

bool SingleDeselect::onToggled(bool state) {
    onEditor();
    return true;
}

bool SingleDeselect::onSettingChanged(std::string_view key, const matjson::Value& value) {
    return true;
}

void SDEditorUI::deselectSpecificObject(CCPoint pos) {
    auto mousePosToNode = m_editorLayer->m_objectLayer->convertToNodeSpace(pos);
    int currentLayer = m_editorLayer->m_currentLayer;

    for (auto object : CCArrayExt<GameObject*>(m_selectedObjects)) {
        bool isOnCurrentEditorLayer1 = object->m_editorLayer == m_editorLayer->m_currentLayer;
        bool isOnCurrentEditorLayer2 = (object->m_editorLayer2 == m_editorLayer->m_currentLayer) && object->m_editorLayer2 != 0;

        bool locked = false;

        auto max = std::max(object->m_editorLayer, object->m_editorLayer2);

        if (m_editorLayer->m_lockedLayers.size() > max) {
            locked = m_editorLayer->m_lockedLayers[object->m_editorLayer] || (object->m_editorLayer2 != 0 && m_editorLayer->m_lockedLayers[object->m_editorLayer2]);
        }

        if (object->boundingBox().containsPoint(mousePosToNode) && !locked && (currentLayer == -1 || (isOnCurrentEditorLayer1 || isOnCurrentEditorLayer2))) {
            deselectObject(object);
            break;
        }
    }
}

void SDEditorUI::selectObject(GameObject* object, bool ignoreFilter) {
    if (!getKeyPressed()) {
        EditorUI::selectObject(object, ignoreFilter);
    }
}

void SDEditorUI::selectObjects(CCArray* objects, bool ignoreFilter) {
    if (!getKeyPressed()) {
        EditorUI::selectObjects(objects, ignoreFilter);
        return;
    }
    for (auto obj : objects->asExt<GameObject>()) {
        deselectObject(obj);
    }
}

void SDEditorUI::ccTouchEnded(CCTouch* touch, CCEvent* event) {
    if (m_selectedMode == 3 && getKeyPressed()) {
        deselectSpecificObject(touch->getLocation());
    }
    EditorUI::ccTouchEnded(touch, event);
}

bool SDEditorUI::getKeyPressed() {
    return SingleDeselect::get()->m_keyHeld && !SingleDeselect::get()->m_blockDeselect;
}

CCArray* SDEditorUI::pasteObjects(gd::string str, bool withColor, bool noUndo) {
    SingleDeselect::get()->m_blockDeselect = true;
    auto ret = EditorUI::pasteObjects(str, withColor, noUndo);
    SingleDeselect::get()->m_blockDeselect = false;
    return ret;
}

void SingleDeselect::onEditor() {
    m_editorUI->removeEventListener("deselect-listener"_spr);
    m_editorUI->addEventListener(
        "deselect-listener"_spr,
        KeybindSettingPressedEventV3(Mod::get(), "SingleDeselect-key"),
        [this](Keybind const& keybind, bool down, bool repeat, double timestamp) {
            m_keyHeld = down;
        }
    );
}