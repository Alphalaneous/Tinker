#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EffectGameObject.hpp>
#include <Geode/modify/SetGroupIDLayer.hpp>
#include <Geode/modify/TeleportPortalObject.hpp>
#include <Geode/modify/EditorUI.hpp>

class $module(FixLinkedTeleport) {};

class $modify(FLTEffectGameObject, EffectGameObject) {
    $registerHooks(FixLinkedTeleport)

    void setOpacity(unsigned char opacity);
};

class $modify(FLTSetGroupIDLayer, SetGroupIDLayer) {
    $registerHooks(FixLinkedTeleport)

    void onClose(cocos2d::CCObject* sender);
};

class $modify(FLTTeleportPortalObject, TeleportPortalObject) {
    $registerHooks(FixLinkedTeleport)

    static TeleportPortalObject* create(char const* frame, bool trigger);
};

class $modify(FLTEditorUI, EditorUI) {
    $registerHooks(FixLinkedTeleport)

    void editGroup(cocos2d::CCObject* sender);
    void editObjectSpecial(int type);
    void transformObject(GameObject* object, EditCommand command, bool noOffset);
    UndoObject* createUndoObject(UndoCommand command, bool addToList);
};
