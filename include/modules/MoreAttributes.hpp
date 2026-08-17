#pragma once

#include "module/Module.hpp"
#include <Geode/modify/SetupKeyframePopup.hpp>
#include <Geode/modify/SetupAdvFollowPopup.hpp>
#include <Geode/modify/SetupInstantCollisionTriggerPopup.hpp>
#include <Geode/modify/SetupObjectOptions2Popup.hpp>
#include <Geode/modify/SetupTriggerPopup.hpp>

class $module(MoreAttributes) {
    bool onToggled(bool state);
};

class $modify(MASetupKeyframePopup, SetupKeyframePopup) {
    $registerHooks(MoreAttributes)

    bool init(KeyframeGameObject* object, cocos2d::CCArray* objects, LevelEditorLayer* layer);
    void valueDidChange(int tag, float value);
};

class $modify(MASetupAdvFollowPopup, SetupAdvFollowPopup) {
    $registerHooks(MoreAttributes)

    bool init(AdvancedFollowTriggerObject* object, cocos2d::CCArray* objects);
    void onCustomToggleTriggerValue(cocos2d::CCObject* sender);
};

class $modify(MASetupInstantCollisionTriggerPopup, SetupInstantCollisionTriggerPopup) {
    $registerHooks(MoreAttributes)

    bool init(EffectGameObject* object, cocos2d::CCArray* objects);
    void onRemapFix(CCObject* sender);
};

class $modify(MASetupObjectOptions2Popup, SetupObjectOptions2Popup) {
    $registerHooks(MoreAttributes)

    bool init(GameObject* object, cocos2d::CCArray* objects);
};

class $modify(MASetupTriggerPopup, SetupTriggerPopup) {
    $registerHooks(MoreAttributes)

    void createMultiTriggerItems(cocos2d::CCPoint touchPos, cocos2d::CCPoint spawnPos, cocos2d::CCPoint multiPos);
    cocos2d::CCArray* createCustomToggleValueControl(int property, bool toggled, bool notClickable, gd::string text, cocos2d::CCPoint position, bool vertical, int page, int group);
    cocos2d::CCArray* createToggleValueControl(int property, gd::string label, cocos2d::CCPoint position, bool vertical, int page, int group, float scale);
};