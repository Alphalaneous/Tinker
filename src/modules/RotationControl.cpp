#include "modules/RotationControl.hpp"
#include <Geode/binding/EditorUI.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <alphalaneous.editortab_api/API.hpp>

using namespace geode::prelude;

bool RotationControl::onSettingChanged(std::string_view key, const matjson::Value& value) {
    auto editor = static_cast<RCEditorUI*>(getEditor());
    auto state = value.asBool().unwrapOrDefault();

    auto rotationControl = editor->m_fields->m_rotationXYControls;

    if (key == "use-refresh-button") {
        rotationControl->updateUI(state);
        return true;
    }
    if (key == "follow-ui-scale") {
        float controlScale = RotationControl::getSetting<float, "control-scale">();

        rotationControl->updateScale(state, controlScale);
        return true;
    }
    if (key == "control-scale") {
        bool followUIScale = RotationControl::getSetting<bool, "follow-ui-scale">();
        float controlScale = value.asFloat().unwrapOrDefault();

        rotationControl->updateScale(followUIScale, controlScale);
        return true;
    }

    return false;
}

void RotationControl::onEditor() {
    auto mode = alpha::editor_tabs::getMode(alpha::editor_tabs::Edit);
    if (!mode) return;

    auto tab = mode->getTab("edit");
    if (!tab) return;

    auto node = tab->getNode();
    if (!node) return;

    auto editorUI = static_cast<RCEditorUI*>(getEditor());
    auto fields = editorUI->m_fields.self();

    fields->m_rotationXYControlsButton = alpha::editor_tabs::createEditButton(CCSprite::create("rotate-xy.png"_spr), 1, [this, editorUI] (auto sender) {
        editorUI->activateRotationXYControl();
    });
    fields->m_rotationXYControlsButton->setID("rotation-xy-toggle"_spr);

    auto warp = node->findItemByID("warp-button");

    node->insertItemAfter(fields->m_rotationXYControlsButton, warp);

    fields->m_rotationXYControls = XYRotationControl::create();
    fields->m_rotationXYControls->setVisible(false);

    editorUI->m_editorLayer->m_objectLayer->addChild(fields->m_rotationXYControls);

    addEventListener(ObjectMovedEvent(), [editorUI, fields] {
        if (!editorUI->m_selectedObject && (editorUI->m_selectedObjects && editorUI->m_selectedObjects->count() == 0)) {
            editorUI->deactivateRotationXYControl();
        }
    });
}

void RCEditorUI::activateRotationXYControl() {
    auto fields = m_fields.self();

    if (m_selectedObjects->count() == 0 && !m_selectedObject) return;

    deactivateTransformControl();
    deactivateScaleControl();
    deactivateRotationControl();
    
    if (fields->m_rotationXYControls->isVisible()) {
        deactivateRotationXYControl();
        return;
    }

    if (fields->m_rotationXYControls->isVisible() && !m_canActivateControls) return;

    if (m_selectedObject) {
        if (m_selectedObject->m_objectType == GameObjectType::Solid) return;
    }
    if (m_selectedObjects) {
        for (auto obj : m_selectedObjects->asExt<GameObject>()) {
            if (obj->m_objectType == GameObjectType::Solid) return;
        }
    }

    GameManager::get()->setGameVariable(GameVar::EnableRotate, false);
    auto sprite = static_cast<ButtonSprite*>(m_rotateBtn->getNormalImage());
    sprite->updateBGImage("GJ_button_01.png");

    m_canActivateControls = false;
    
    if (fields->m_rotationXYControlsButton) {
        auto btnSprite = fields->m_rotationXYControlsButton->getChildByType<ButtonSprite>();
        if (btnSprite) btnSprite->setColor({166, 166, 166});
    }

    m_pivotPoint = getSelectionCenter();
    
    fields->m_rotationXYControls->setPosition(m_pivotPoint);
    fields->m_rotationXYControls->setScale(1.f / m_editorLayer->m_objectLayer->getScale());
    fields->m_rotationXYControls->setVisible(true);

    updateRotationXYControl();
}

void RCEditorUI::deactivateRotationXYControl() {
    auto fields = m_fields.self();
    if (!fields->m_rotationXYControls) return;
    
    if (fields->m_rotationXYControls->isVisible()) {
        fields->m_rotationXYControls->setVisible(false);
        fields->m_rotationXYControls->unfocus();
    }
    if (fields->m_rotationXYControlsButton) {
        auto btnSprite = fields->m_rotationXYControlsButton->getChildByType<ButtonSprite>(0);
        if (btnSprite) btnSprite->setColor({255, 255, 255});
    }
}

void RCEditorUI::updateRotationXYControl() {
    auto fields = m_fields.self();
    if (!fields->m_rotationXYControls->isVisible()) return;

    auto control = fields->m_rotationXYControls;
    control->loadValues(m_selectedObject, m_selectedObjects, m_objectEditorStates);
}

void RCEditorUI::updateTransformControl() {
    EditorUI::updateTransformControl();
    updateRotationXYControl();
}

CCPoint RCEditorUI::getSelectionCenter() {
    CCPoint position{};

    if (m_selectedObject) {
        position = m_selectedObject->getPosition();
    }
    else if (m_selectedObjects && m_selectedObjects->count() != 0) {
        position = getGroupCenter(m_selectedObjects, false);
    }

    return position;
}

void RCEditorUI::updateButtons() {
    EditorUI::updateButtons();
    auto fields = m_fields.self();

    if (fields->m_rotationXYControls) {
        fields->m_rotationXYControls->setPosition(getSelectionCenter());

        if (!m_selectedObject && (m_selectedObjects && m_selectedObjects->count() == 0)) {
            deactivateRotationXYControl();
        }
    }
}

void RCEditorUI::activateScaleControl(CCObject* sender) {
    deactivateRotationXYControl();
    EditorUI::activateScaleControl(sender);
}

void RCEditorUI::activateTransformControl(CCObject* sender) {
    deactivateRotationXYControl();
    EditorUI::activateTransformControl(sender);
}

void RCEditorUI::activateRotationControl(CCObject* sender) {
    deactivateRotationXYControl();
    EditorUI::activateRotationControl(sender);
}

void RCEditorUI::selectAllWithDirection(bool left) {
    EditorUI::selectAllWithDirection(left);
    deactivateRotationXYControl();
}

void RCEditorUI::deleteSmartBlocksFromObjects(CCArray* objects) {
    EditorUI::deleteSmartBlocksFromObjects(objects);
    deactivateRotationXYControl();
}

void RCEditorUI::deselectAll() {
    EditorUI::deselectAll();
    deactivateRotationXYControl();
}

void RCLevelEditorLayer::undoLastAction() {
    LevelEditorLayer::undoLastAction();
    static_cast<RCEditorUI*>(m_editorUI)->updateRotationXYControl();
}

void RCLevelEditorLayer::redoLastAction() {
    LevelEditorLayer::redoLastAction();
    static_cast<RCEditorUI*>(m_editorUI)->updateRotationXYControl();

}