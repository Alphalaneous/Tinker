#include "modules/fixes/FixLinkedTeleport.hpp"
#include "utils/Constants.hpp"

void FLTEffectGameObject::setOpacity(unsigned char opacity) {
    if (m_objectID == tinker::constants::objects::LinkedOrangeTeleportPortal) {
        return;
    }
    if (m_objectID == tinker::constants::objects::LinkedTeleportPortal) {
        auto teleport = reinterpret_cast<TeleportPortalObject*>(this);
        if (teleport->m_orangePortal->m_isSelected) {
            opacity = 255;
        }
        teleport->m_orangePortal->EffectGameObject::setOpacity(opacity);
    }
    EffectGameObject::setOpacity(opacity);
}

void FLTSetGroupIDLayer::onClose(cocos2d::CCObject* sender) {
    SetGroupIDLayer::onClose(sender);

    if (m_targetObject && m_targetObject->m_objectID == tinker::constants::objects::LinkedTeleportPortal) {
        auto teleport = reinterpret_cast<TeleportPortalObject*>(m_targetObject);
        teleport->m_orangePortal->m_editorLayer = teleport->m_editorLayer;
        teleport->m_orangePortal->m_editorLayer2 = teleport->m_editorLayer2;
    }
    if (m_targetObjects && m_targetObjects->count() > 0) {
        for (auto obj : m_targetObjects->asExt<GameObject>()) {
            if (obj->m_objectID == tinker::constants::objects::LinkedTeleportPortal) {
                auto teleport = reinterpret_cast<TeleportPortalObject*>(obj);
                teleport->m_orangePortal->m_editorLayer = teleport->m_editorLayer;
                teleport->m_orangePortal->m_editorLayer2 = teleport->m_editorLayer2;  
            }
        }
    }
}


TeleportPortalObject* FLTTeleportPortalObject::create(char const* frame, bool trigger) {
    auto ret = TeleportPortalObject::create(frame, trigger);

    ret->runAction(CallFuncExt::create([ret] {
        if (!ret->m_isTrigger && ret->m_orangePortal) {
            ret->m_orangePortal->m_editorLayer = ret->m_editorLayer;
            ret->m_orangePortal->m_editorLayer2 = ret->m_editorLayer2;

            ret->m_orangePortal->setUserObject("teleport-owner"_spr, ret);
        }
    }));

    return ret;
}

void FLTEditorUI::editGroup(cocos2d::CCObject* sender) {
    if (m_selectedObject && m_selectedObject->m_objectID == tinker::constants::objects::LinkedOrangeTeleportPortal) {
        auto owner = static_cast<TeleportPortalObject*>(m_selectedObject->getUserObject("teleport-owner"_spr));
        auto oldSelected = m_selectedObject;
        m_selectedObject = owner;
        EditorUI::editGroup(sender);
        m_selectedObject = oldSelected;
        return;
    }

    if (m_selectedObjects && m_selectedObjects->count() > 0) {
        std::unordered_set<GameObject*> bluePortals;
        std::unordered_set<GameObject*> orangePortals;

        for (auto obj : m_selectedObjects->asExt<GameObject>()) {
            if (obj->m_objectID == tinker::constants::objects::LinkedTeleportPortal) {
                bluePortals.insert(obj);
            }
            if (obj->m_objectID == tinker::constants::objects::LinkedOrangeTeleportPortal) {
                orangePortals.insert(obj);
            }
        }

        if (m_selectedObjects->count() == (bluePortals.size() + orangePortals.size())) {
            for (auto orangePortal : orangePortals) {
                auto owner = static_cast<GameObject*>(orangePortal->getUserObject("teleport-owner"_spr));
                if (!bluePortals.contains(owner)) {
                    return;
                }
            }
        }
    }

    EditorUI::editGroup(sender);
}

void FLTEditorUI::editObjectSpecial(int type) {
    if (m_selectedObject && m_selectedObject->m_objectID == tinker::constants::objects::LinkedOrangeTeleportPortal) {
        auto owner = static_cast<TeleportPortalObject*>(m_selectedObject->getUserObject("teleport-owner"_spr));
        auto oldSelected = m_selectedObject;
        m_selectedObject = owner;
        EditorUI::editObjectSpecial(type);
        m_selectedObject = oldSelected;
        return;
    }

    if (m_selectedObjects && m_selectedObjects->count() > 0) {
        std::unordered_set<GameObject*> bluePortals;
        std::unordered_set<GameObject*> orangePortals;

        for (auto obj : m_selectedObjects->asExt<GameObject>()) {
            if (obj->m_objectID == tinker::constants::objects::LinkedTeleportPortal) {
                bluePortals.insert(obj);
            }
            if (obj->m_objectID == tinker::constants::objects::LinkedOrangeTeleportPortal) {
                orangePortals.insert(obj);
            }
        }

        if (m_selectedObjects->count() == (bluePortals.size() + orangePortals.size())) {
            bool canEditSpecialPair = true;

            for (auto orangePortal : orangePortals) {
                auto owner = static_cast<GameObject*>(orangePortal->getUserObject("teleport-owner"_spr));
                if (!bluePortals.contains(owner)) {
                    canEditSpecialPair = false;
                    return;
                }
            }

            if (canEditSpecialPair) {
                auto selected = m_selectedObjects;
                m_selectedObjects = m_selectedObjects->shallowCopy();

                for (int i = m_selectedObjects->count() - 1; i >= 0; i--) {
                    if (orangePortals.contains(static_cast<GameObject*>(m_selectedObjects->objectAtIndex(i)))) {
                        m_selectedObjects->removeObjectAtIndex(i);
                    }
                }

                EditorUI::editObjectSpecial(type);
                m_selectedObjects = selected;
                return;
            }
        }
    }

    EditorUI::editObjectSpecial(type);
}

void FLTEditorUI::transformObject(GameObject* object, EditCommand command, bool noOffset) {
    if (object->m_objectID == tinker::constants::objects::LinkedTeleportPortal) {
        auto child = static_cast<TeleportPortalObject*>(object)->m_orangePortal;
        if (child) {
            EditorUI::transformObject(child, command, noOffset);
        }
        EditorUI::transformObject(object, command, noOffset);
        return;
    }
    if (object->m_objectID == tinker::constants::objects::LinkedOrangeTeleportPortal) {
        auto owner = static_cast<GameObject*>(object->getUserObject("teleport-owner"_spr));
        if (owner) {
            transformObject(owner, command, noOffset);
        }
        return;
    }
    EditorUI::transformObject(object, command, noOffset);
}

UndoObject* FLTEditorUI::createUndoObject(UndoCommand command, bool addToList) {
    if (m_selectedObject && m_selectedObject->m_objectID == tinker::constants::objects::LinkedOrangeTeleportPortal) {        
        auto oldSelected = m_selectedObject;
        auto owner = static_cast<GameObject*>(m_selectedObject->getUserObject("teleport-owner"_spr));
        if (owner) m_selectedObject = owner;
        
        auto ret = EditorUI::createUndoObject(command, addToList);
        m_selectedObject = oldSelected;
        return ret;
    }
    return EditorUI::createUndoObject(command, addToList);
}
