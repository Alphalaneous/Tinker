#include "QuickExtras.hpp"

void QuickExtras::onEditor() {
    if (auto editorButtonsMenu = m_editorUI->getChildByID("editor-buttons-menu")) {
        auto spr = CCSprite::create("edit_extras.png"_spr);

        if (!getSetting<bool, "always-show">()) {
            m_editorUI->m_editSpecialBtn->setSprite(spr);
        }
        else {
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

            editorButtonsMenu->addChild(m_editExtrasBtn);
            m_editorUI->m_uiItems->addObject(m_editExtrasBtn);
            editorButtonsMenu->updateLayout();
        }

    }

    addEventListener(UpdateButtonsEvent(), [this] {
        bool selected = m_editorUI->m_selectedObject || m_editorUI->m_selectedObjects->count() != 0;

        if (!getSetting<bool, "always-show">()) {
            bool isSpecial = static_cast<QEEditorUI*>(m_editorUI)->_editButton2Usable();

            if (isSpecial) {
                auto spr = CCSprite::createWithSpriteFrameName("GJ_editObjBtn4_001.png");
                m_editorUI->m_editSpecialBtn->setSprite(spr);
            }
            else {
                auto spr = CCSprite::create("edit_extras.png"_spr);
                m_editorUI->m_editSpecialBtn->setSprite(spr);
                spr->setAnchorPoint({0, 0});
                spr->setPosition({1, 0});
            }

            m_editorUI->m_editSpecialBtn->setContentSize({40, 40});
            m_editorUI->m_editSpecialBtn->setColor(selected ? ccColor3B{255, 255, 255} : ccColor3B{166, 166, 166});
            m_editorUI->m_editSpecialBtn->setOpacity(selected ? 255 : 175);
            m_editorUI->m_editSpecialBtn->m_animationEnabled = selected;

            if (auto editorButtonsMenu = m_editorUI->getChildByID("editor-buttons-menu")) {
                editorButtonsMenu->updateLayout();
            }
        }
        
        if (!m_editExtrasBtn) return;

        m_editExtrasBtn->setColor(selected ? ccColor3B{255, 255, 255} : ccColor3B{166, 166, 166});
        m_editExtrasBtn->setOpacity(selected ? 255 : 175);
        m_editExtrasBtn->m_animationEnabled = selected;
    });
}

void QuickExtras::onEditExtras() {
    if (m_editorUI->m_selectedObject || m_editorUI->m_selectedObjects->count() > 0) {
        auto idLayer = SetGroupIDLayer::create(m_editorUI->m_selectedObject, m_editorUI->m_selectedObjects);
        auto popup = SetupObjectOptionsPopup::create(m_editorUI->m_selectedObject, m_editorUI->m_selectedObjects, idLayer);
        popup->show();
    }
}

void QEEditorUI::editObjectSpecial(int type) {
    if (_editButton2Usable()) {
        EditorUI::editObjectSpecial(type);
    }
    else {
        QuickExtras::get()->onEditExtras();
    }
}

bool QEEditorUI::_isAllowedObjectID(int id) {
    return id == 3643 // toggle block
        || id == 1594 // toggle orb
        || id == 2903 // gradient
        || id == 1615 // counter
        || id == 1704 // green dash orb
        || id == 1751 // pink dash orb
        || id == 2063  // checkpoint
        || id == 2065 // particle
        || id == 1329 // user coin
        || id == 142; // gold coin
}

bool QEEditorUI::isDisallowedObjectID(int id) {
    return id == 2064 // orange teleport portal
        || id == 749 // linked orange teleport portal
        || id == 918 // monster chompy
        || id == 1327 // small monster
        || id == 1328 // medium monster
        || id == 1584 // bat monster
        || id == 2012 // spike ball monster
        || id == 919; // animated spike pit
}

bool QEEditorUI::isSpecialEdit(GameObject* obj) {
    if (!obj) return false;
    if (isDisallowedObjectID(obj->m_objectID)) return false;
        
    if (obj->m_classType == GameObjectClassType::Enhanced) {
        auto enhanced = static_cast<EnhancedGameObject*>(obj);
        if (enhanced->m_hasCustomAnimation) return true;
        if (enhanced->m_hasCustomRotation) return true;
    }
    
    if (_isAllowedObjectID(obj->m_objectID)) return true;
    if (obj->getType() == GameObjectType::Collectible) return true;
    if (obj->isSpecialObject()) return true;
    if (obj->canAllowMultiActivate()) return true;

    return false;
}

bool QEEditorUI::_checkMultiSelection() {
    if (m_selectedObjects->count() == 0) return false;

    bool special = true;

    for (auto object : m_selectedObjects->asExt<GameObject>()) {
        if (object->m_classType == GameObjectClassType::Smart) {
            return true;
        }
        if (!isSpecialEdit(object)) {
            special = false;
        }
    }

    return special;
}

bool QEEditorUI::_editButton2Usable() {
    int objectID = -1;
    int classType = -1;
    int objectType = -1;

    getGroupInfo(m_selectedObject, m_selectedObjects, objectID, classType, objectType);

    if (objectID == -1 && classType == -1) return false;
    if (classType == static_cast<int>(GameObjectClassType::Smart)) return true;
    if (objectType == static_cast<int>(GameObjectType::Collectible)) return true;

    if (!m_selectedObject) return _checkMultiSelection();

    if (isSpecialEdit(m_selectedObject)) return true;

    return false;
}