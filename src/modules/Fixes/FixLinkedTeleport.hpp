#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EffectGameObject.hpp>
#include <Geode/modify/SetGroupIDLayer.hpp>
#include <Geode/modify/TeleportPortalObject.hpp>
#include <Geode/modify/EditorUI.hpp>

class $editorModule(FixLinkedTeleport) {};

class $modify(FLTEffectGameObject, EffectGameObject) {
    $registerEditorHooks(FixLinkedTeleport)

    void setOpacity(unsigned char opacity);
};

class $modify(FLTSetGroupIDLayer, SetGroupIDLayer) {
    $registerEditorHooks(FixLinkedTeleport)

    void onClose(cocos2d::CCObject* sender);
};

class $modify(FLTTeleportPortalObject, TeleportPortalObject) {
    $registerEditorHooks(FixLinkedTeleport)

    void customObjectSetup(gd::vector<gd::string>& values, gd::vector<void*>& exists);
};

class $modify(FLTEditorUI, EditorUI) {
    $registerEditorHooks(FixLinkedTeleport)

    void editGroup(cocos2d::CCObject* sender);
    void editObjectSpecial(int type);
    void transformObject(GameObject* object, EditCommand command, bool noOffset);
    UndoObject* createUndoObject(UndoCommand command, bool addToList);
};
