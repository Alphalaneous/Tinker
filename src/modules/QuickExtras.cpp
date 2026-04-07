#include "QuickExtras.hpp"

void QuickExtras::onEditor() {
    if (auto editorButtonsMenu = m_editorUI->getChildByID("editor-buttons-menu")) {
        auto spr = CCSprite::create("edit_extras.png"_spr);
        
        m_editExtrasBtn = CCMenuItemExt::createSpriteExtra(spr, [this] (auto sender) {
            onEditExtras();
        });

        m_editExtrasBtn->setContentSize({40, 40});
        m_editExtrasBtn->setOpacity(175);
        m_editExtrasBtn->setColor({166, 166, 166});
        m_editExtrasBtn->m_animationEnabled = false;
        m_editExtrasBtn->setID("edit-extras-button"_spr);

        spr->setAnchorPoint({0, 0});
        spr->setPosition({1, 0});

        m_editorUI->m_uiItems->addObject(m_editExtrasBtn);

        if (!getSetting<bool, "always-show">()) {
            editorButtonsMenu->insertBefore(m_editExtrasBtn, m_editorUI->m_editSpecialBtn);
            m_editorUI->m_editSpecialBtn->setVisible(false);
        }
        else {
            editorButtonsMenu->addChild(m_editExtrasBtn);
            m_editorUI->m_uiItems->addObject(m_editExtrasBtn);
        }

        editorButtonsMenu->updateLayout();
    }
}

void QuickExtras::onUpdateButtons() {
    if (!m_editExtrasBtn) return;

    if (!getSetting<bool, "always-show">()) {
        bool isSpecial = static_cast<QEEditorUI*>(m_editorUI)->_editButton2Usable();

        m_editExtrasBtn->setVisible(!isSpecial);
        m_editorUI->m_editSpecialBtn->setVisible(isSpecial);

        if (auto editorButtonsMenu = m_editorUI->getChildByID("editor-buttons-menu")) {
            editorButtonsMenu->updateLayout();
        }
    }

    if (m_editorUI->m_selectedObject || m_editorUI->m_selectedObjects->count() > 0) {
        m_editExtrasBtn->setOpacity(255);
        m_editExtrasBtn->setColor({255, 255, 255});
        m_editExtrasBtn->m_animationEnabled = true;
    }
    else {
        m_editExtrasBtn->setOpacity(175);
        m_editExtrasBtn->setColor({166, 166, 166});
        m_editExtrasBtn->m_animationEnabled = false;
    }
}

void QuickExtras::onEditExtras() {
    if (m_editorUI->m_selectedObject || m_editorUI->m_selectedObjects->count() > 0) {
        auto idLayer = SetGroupIDLayer::create(m_editorUI->m_selectedObject, m_editorUI->m_selectedObjects);
        auto popup = SetupObjectOptionsPopup::create(m_editorUI->m_selectedObject, m_editorUI->m_selectedObjects, idLayer);
        popup->show();
    }
}

bool QEEditorUI::_isAllowedObjectID(int id) {
    return id == 2907 
        || (id >= 2909 && id <= 2917) 
        || (id >= 2919 && id <= 2924) 
        || id == 3643 
        || id == 1594 
        || id == 1615 
        || id == 1704 
        || id == 1751 
        || id == 2063 
        || id == 2065;
}

bool QEEditorUI::isSpecialEdit(GameObject* obj) {
    if (!obj) return false;

    if (_isAllowedObjectID(obj->m_objectID)) return true;
    if (obj->getType() == GameObjectType::Collectible) return true;
    if (obj->isSpecialObject()) return true;
    if (obj->canMultiActivate(true)) return true;
    if (obj->isTrigger()) return true;

    return false;
}

bool QEEditorUI::_checkMultiSelection() {
    if (m_selectedObjects->count() == 0) return false;

    for (auto object : m_selectedObjects->asExt<GameObject>()) {
        if (!isSpecialEdit(object)) return false;
    }

    return true;
}

bool QEEditorUI::_editButton2Usable() {
    int objectID = -1;
    int classType = -1;
    int objectType = -1;

    getGroupInfo(m_selectedObject, m_selectedObjects, objectID, classType, objectType);

    if (objectID == -1 && classType == -1) return false;
    if (classType == static_cast<int>(GameObjectClassType::Smart)) return true;

    bool isSmart = arrayContainsClass(
        m_selectedObjects,
        static_cast<int>(GameObjectClassType::Smart)
    );

    if (isSmart || objectType == static_cast<int>(GameObjectType::Collectible)) return true;

    if (!m_selectedObject) return _checkMultiSelection();

    if (isSpecialEdit(m_selectedObject)) return true;

    return false;
}

void QEEditorUI::showUI(bool show) {
    EditorUI::showUI(show);

    if (QuickExtras::getSetting<bool, "always-show">()) return;

    if (auto qeBtn = QuickExtras::get()->m_editExtrasBtn) {
        bool isSpecial = _editButton2Usable();

        qeBtn->setVisible(!isSpecial && show);
        m_editSpecialBtn->setVisible(isSpecial && show);
    }
}