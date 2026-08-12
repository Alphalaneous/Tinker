#include "modules/QuickExtras.hpp"
#include "utils/Constants.hpp"
#include <smjs.object-collab/include/object_collab_optional.hpp>

void QuickExtras::onEditor() {
    if (auto editorButtonsMenu = getEditor()->getChildByID("editor-buttons-menu")) {
        auto spr = CCSprite::create("edit_extras.png"_spr);

        if (!getSetting<bool, "always-show">()) {
            getEditor()->m_editSpecialBtn->setSprite(spr);
        }
        else {
            m_editExtrasBtn = CCMenuItemExt::createSpriteExtra(spr, [this] (auto sender) {
                onEditExtras();
            });

            m_editExtrasBtn->setContentSize({40.f, 40.f});
            m_editExtrasBtn->setOpacity(175);
            m_editExtrasBtn->setColor({166, 166, 166});
            m_editExtrasBtn->m_animationEnabled = false;
            m_editExtrasBtn->setID("edit-extras-button"_spr);

            spr->setAnchorPoint({0.f, 0.f});
            spr->setPosition({1.f, 0.f});

            editorButtonsMenu->addChild(m_editExtrasBtn);
            getEditor()->m_uiItems->addObject(m_editExtrasBtn);
            editorButtonsMenu->updateLayout();
        }

    }

    addEventListener(UpdateButtonsEvent(), [this] {
        bool selected = getEditor()->m_selectedObject || getEditor()->m_selectedObjects->count() != 0;

        if (!getSetting<bool, "always-show">()) {
            auto registryRes = object_collab::getOptionalRegister();


            bool isSpecial = static_cast<QEEditorUI*>(getEditor())->_editButton2Usable();
            if (getEditor()->m_selectedObjects && getEditor()->m_selectedObjects->count() > 0) {
                for (auto obj : getEditor()->m_selectedObjects->asExt<GameObject>()) {
                    if (obj->m_objectID >= 100000000) {
                        if (registryRes) {
                            auto registry = registryRes.unwrap();
                            auto& info = registry[obj->m_objectID];
                            if (info.hasEditSpecial) {
                                isSpecial = true;
                                break;
                            }
                        }
                    }
                }
            }
            else if (getEditor()->m_selectedObject) {
                if (getEditor()->m_selectedObject->m_objectID >= 100000000) {
                    if (registryRes) {
                        auto registry = registryRes.unwrap();
                        auto& info = registry[getEditor()->m_selectedObject->m_objectID];
                        if (info.hasEditSpecial) {
                            isSpecial = true;
                        }
                    }
                }
            }

            if (isSpecial) {
                auto spr = CCSprite::createWithSpriteFrameName("GJ_editObjBtn4_001.png");
                getEditor()->m_editSpecialBtn->setSprite(spr);
            }
            else {
                auto spr = CCSprite::create("edit_extras.png"_spr);
                getEditor()->m_editSpecialBtn->setSprite(spr);
                spr->setAnchorPoint({0.f, 0.f});
                spr->setPosition({1.f, 0.f});
            }

            getEditor()->m_editSpecialBtn->setContentSize({40.f, 40.f});
            getEditor()->m_editSpecialBtn->setColor(selected ? ccColor3B{255, 255, 255} : ccColor3B{166, 166, 166});
            getEditor()->m_editSpecialBtn->setOpacity(selected ? 255 : 175);
            getEditor()->m_editSpecialBtn->setEnabled(selected);
            getEditor()->m_editSpecialBtn->m_animationEnabled = selected;

            if (auto editorButtonsMenu = getEditor()->getChildByID("editor-buttons-menu")) {
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
    if (getEditor()->m_selectedObject || getEditor()->m_selectedObjects->count() > 0) {
        auto idLayer = SetGroupIDLayer::create(getEditor()->m_selectedObject, getEditor()->m_selectedObjects);
        auto popup = SetupObjectOptionsPopup::create(getEditor()->m_selectedObject, getEditor()->m_selectedObjects, idLayer);
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
    using namespace tinker::constants::objects;

    return id == PlayerTouchToggle
        || id == LinkedOrangeTeleportPortal
        || id == ToggleOrb
        || id == GradientTrigger
        || id == Counter
        || id == GreenDashOrb
        || id == PinkDashOrb
        || id == Checkpoint
        || id == Particle
        || id == UserCoin
        || id == GoldCoin;
}

bool QEEditorUI::isDisallowedObjectID(int id) {
    using namespace tinker::constants::objects;

    return id == OrangeTeleportPortal
        || id == LargeBeast
        || id == SmallMonster
        || id == MediumMonster
        || id == BatMonster
        || id == SpikeBallMonster
        || id == AnimatedBlackPit
        || id == Text;
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