#include "RotationSliders.hpp"
#include <Geode/Enums.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/GameObject.hpp>

void RotationSliders::onEditor() {
    auto editorUI = static_cast<RSEditorUI*>(m_editorUI);
    auto fields = editorUI->m_fields.self();

    fields->m_rotationXYControlsButton = m_editorUI->getSpriteButton("center-object.png"_spr, menu_selector(RSEditorUI::activateRotationXYControl), nullptr, 0.9f);
    fields->m_rotationXYControlsButton->setID("rotation-xy-toggle"_spr);
    m_editorUI->m_editButtonBar->m_buttonArray->addObject(fields->m_rotationXYControlsButton);

    auto cols = GameManager::get()->getIntGameVariable(GameVar::EditorButtonsPerRow);
    auto rows = GameManager::get()->getIntGameVariable(GameVar::EditorButtonRows);

    m_editorUI->m_editButtonBar->reloadItems(cols, rows);

    fields->m_rotationXYControls = XYRotationControl::create();
    fields->m_rotationXYControls->setVisible(false);

    m_editorLayer->m_objectLayer->addChild(fields->m_rotationXYControls);

    addEventListener(ObjectMovedEvent(), [editorUI, fields] () {
        if (fields->m_rotationXYControls) {
            fields->m_rotationXYControls->setPosition(editorUI->getSelectionCenter());

            fields->m_rotationXYControls->loadValues(editorUI->m_selectedObject, editorUI->m_selectedObjects, editorUI->m_objectEditorStates);

            if (!editorUI->m_selectedObject && (editorUI->m_selectedObjects && editorUI->m_selectedObjects->count() == 0)) {
                editorUI->deactivateRotationXYControl();
            }
        }
    });
}

void RSEditorUI::activateRotationXYControl(CCObject* sender) {
    auto fields = m_fields.self();

    if ((m_selectedObjects->count() != 0 || m_selectedObject) && (!m_rotationControl->isVisible() || m_canActivateControls)) {
        deactivateTransformControl();
        deactivateScaleControl();
        deactivateRotationControl();
        
        if (fields->m_rotationXYControls->isVisible()) {
            deactivateRotationXYControl();
        }
        else {
            if (m_selectedObject) {
                if (m_selectedObject->m_objectType == GameObjectType::Solid) return;
            }
            if (m_selectedObjects) {
                for (auto obj : m_selectedObjects->asExt<GameObject>()) {
                    if (obj->m_objectType == GameObjectType::Solid) return;
                }
            }

            m_canActivateControls = false;
            
            if (fields->m_rotationXYControlsButton) {
                auto btnSprite = fields->m_rotationXYControlsButton->getChildByType<ButtonSprite>(0);
                if (btnSprite) btnSprite->setColor({166, 166, 166});
            }

            fields->m_rotationXYControls->setPosition(getSelectionCenter());

            auto scale = m_editorLayer->m_objectLayer->getScale();

            fields->m_rotationXYControls->setScale(1.f / scale);
            fields->m_rotationXYControls->setVisible(true);

            fields->m_rotationXYControls->loadValues(m_selectedObject, m_selectedObjects, m_objectEditorStates);
        }
    }
}

void RSEditorUI::deactivateRotationXYControl() {
    auto fields = m_fields.self();
    if (fields->m_rotationXYControls->isVisible()) {
        fields->m_rotationXYControls->setVisible(false);
        fields->m_rotationXYControls->unfocus();
    }
    if (fields->m_rotationXYControlsButton) {
        auto btnSprite = fields->m_rotationXYControlsButton->getChildByType<ButtonSprite>(0);
        if (btnSprite) btnSprite->setColor({255, 255, 255});
    }
}

CCPoint RSEditorUI::getSelectionCenter() {
    CCPoint position;

    if (m_selectedObject) {
        position = m_selectedObject->getPosition();
    }
    else if (m_selectedObjects && m_selectedObjects->count() != 0) {
        position = getGroupCenter(m_selectedObjects, false);
    }

    return position;
}

void RSEditorUI::updateButtons() {
    EditorUI::updateButtons();
    auto fields = m_fields.self();

    if (fields->m_rotationXYControls) {
        fields->m_rotationXYControls->setPosition(getSelectionCenter());

        if (!m_selectedObject && (m_selectedObjects && m_selectedObjects->count() == 0)) {
            deactivateRotationXYControl();
        }
    }
}

void RSEditorUI::activateScaleControl(CCObject* sender) {
    deactivateRotationXYControl();
    EditorUI::activateScaleControl(sender);
}

void RSEditorUI::activateTransformControl(CCObject* sender) {
    deactivateRotationXYControl();
    EditorUI::activateTransformControl(sender);
}

void RSEditorUI::activateRotationControl(CCObject* sender) {
    deactivateRotationXYControl();
    EditorUI::activateRotationControl(sender);
}

/*void RSEditorUI::selectAllWithDirection(bool left) {
    EditorUI::selectAllWithDirection(left);
    deactivateRotationXYControl();
}

void RSEditorUI::deleteSmartBlocksFromObjects(CCArray* objects) {
    EditorUI::deleteSmartBlocksFromObjects(objects);
    deactivateRotationXYControl();
}

void RSEditorUI::deselectAll() {
    EditorUI::deselectAll();
    deactivateRotationXYControl();
}*/