#pragma once

#include "module/Module.hpp"
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/utils/cocos.hpp>
#include "nodes/XYRotationControl.hpp"

using namespace tinker::ui;

class $module(RotationControl) {
    void onEditor();

    bool onSettingChanged(std::string_view key, const matjson::Value& value);

};

class $modify(RCEditorUI, EditorUI) {
    $registerHooks(RotationControl)

    struct Fields {
        geode::Ref<CCMenuItemSpriteExtra> m_rotationXYControlsButton = nullptr;
        geode::Ref<XYRotationControl> m_rotationXYControls = nullptr;
    };

    void activateRotationXYControl();
    void deactivateRotationXYControl();
    void updateRotationXYControl();

    void updateButtons();

    cocos2d::CCPoint getSelectionCenter();

    void updateTransformControl();

    void activateScaleControl(cocos2d::CCObject* sender);
    void activateTransformControl(cocos2d::CCObject* sender);
    void activateRotationControl(cocos2d::CCObject* sender);

    void selectAllWithDirection(bool left);
    void deselectAll();
    void deleteSmartBlocksFromObjects(cocos2d::CCArray* objects);
};

class $modify(RCLevelEditorLayer, LevelEditorLayer) {
    $registerHooks(RotationControl)

    void undoLastAction();
    void redoLastAction();
};