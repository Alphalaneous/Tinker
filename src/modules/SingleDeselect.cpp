#include "modules/SingleDeselect.hpp"

// TODO fix deselecting while controls are open

bool SingleDeselect::onToggled(bool state) {
    if (state) onEditor();
    else removeEventListener("deselect-listener"_spr);
    return true;
}

bool SingleDeselect::onSettingChanged(std::string_view key, const matjson::Value& value) {
    return true;
}

void SDEditorUI::selectObject(GameObject* object, bool ignoreFilter) {
    if (getKeyPressed()) return;
    EditorUI::selectObject(object, ignoreFilter);
}

void SDEditorUI::selectObjects(CCArray* objects, bool ignoreFilter) {
    if (!getKeyPressed()) {
        EditorUI::selectObjects(objects, ignoreFilter);
        return;
    }
    if (objects->count() > 0) {
        auto deselectable = CCArray::create();

        for (auto obj : objects->asExt<GameObject>()) {
            if (m_selectedObject == obj || m_selectedObjects->containsObject(obj)) {
                deselectable->addObject(obj);
            }
        }

        if (deselectable->count() == 0) {
            m_editorLayer->m_undoObjects->removeLastObject();
        }

        if (deselectable->count() > 0) {
            createUndoSelectObject(false);
            for (auto obj : deselectable->asExt<GameObject>()) {
                deselectObject(obj);
            }
        }
    }
}

void SDEditorUI::createUndoSelectObject(bool redo) {
    EditorUI::createUndoSelectObject(redo);
}

GameObject* SDEditorUI::selectedObjectAtPosition(CCPoint pos) {
    CCArray* objs;
    if (m_selectedObject && m_selectedObjects->count() == 0) {
        objs = CCArray::createWithObject(m_selectedObject);
    }
    else {
        objs = m_selectedObjects;
    }

    for (auto obj : CCArrayExt<GameObject, false>(objs)) {
        if (obj->boundingBox().containsPoint(pos)) {
            return obj;
        }
    }
    return nullptr;
}

void SDEditorUI::ccTouchEnded(CCTouch* touch, CCEvent* event) {
    auto world = touch->getLocation();
    bool tapCandidate = m_swipeStart.getDistance(world) < 2.f;

    bool swipeSelected = m_swipeSelected;
    bool swipeActive = m_swipeActive;
    bool canActivateControls = m_canActivateControls;
    bool editingObject = m_editingObject;
    bool isDraggingCamera = m_isDraggingCamera;
    auto swipeStart = m_swipeStart;
    auto snapObject = m_snapObject;
    int selectedMode = m_selectedMode;

    EditorUI::ccTouchEnded(touch, event);

    if ((swipeActive && !tapCandidate) || isDraggingCamera) return;

    float swipeDistance = swipeStart.getDistance(world);

    if (swipeSelected && swipeDistance >= 20.f) return;
    if (m_selectedMode != 3 || !getKeyPressed() || (snapObject && editingObject)) return;
    if (world.y < tinker::utils::getToolbarHeight()) return;

    auto position = m_editorLayer->m_objectLayer->convertToNodeSpace(world);
    auto object = selectedObjectAtPosition(position);

    if (!object || (m_selectedObject != object && !m_selectedObjects->containsObject(object))) return;

    createUndoSelectObject(false);

    if (m_editorLayer->validGroup(object, true)) {
        deselectObject(object);
    }

    if (canActivateControls) {
        updateSpecialUIElements();
        updateTransformControl();
    }

    updateButtons();
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
    addEventListener(
        "deselect-listener"_spr,
        KeybindSettingPressedEvent(Mod::get(), "SingleDeselect-key"),
        [this](Keybind const& keybind, bool down, bool repeat, double timestamp) {
            m_keyHeld = down;
        }
    );
}