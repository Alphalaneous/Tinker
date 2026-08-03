#pragma once

#include "module/Module.hpp"
#include <Geode/modify/SetupKeyframePopup.hpp>
#include <Geode/modify/SetupAdvFollowPopup.hpp>
#include <Geode/modify/SetupInstantCollisionTriggerPopup.hpp>
#include <Geode/modify/SetupObjectOptions2Popup.hpp>

class $editorModule(MoreAttributes) {
};

class $modify(MASetupKeyframePopup, SetupKeyframePopup) {
    $registerEditorHooks(MoreAttributes)

    bool init(KeyframeGameObject* object, cocos2d::CCArray* objects, LevelEditorLayer* layer);
    void valueDidChange(int tag, float value);
};

class $modify(MASetupAdvFollowPopup, SetupAdvFollowPopup) {
    $registerEditorHooks(MoreAttributes)

    bool init(AdvancedFollowTriggerObject* object, cocos2d::CCArray* objects);
    void onCustomToggleTriggerValue(cocos2d::CCObject* sender);
};

class $modify(MASetupInstantCollisionTriggerPopup, SetupInstantCollisionTriggerPopup) {
    $registerEditorHooks(MoreAttributes)

    bool init(EffectGameObject* object, cocos2d::CCArray* objects);
    void onRemapFix(CCObject* sender);
};

class $modify(MASetupObjectOptions2Popup, SetupObjectOptions2Popup) {
    $registerEditorHooks(MoreAttributes)

    bool init(GameObject* object, cocos2d::CCArray* objects);
};