#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>
#include "XYRotationControl.hpp"

using namespace tinker::ui;

class $module(RotationSliders) {
    void onEditor();
};

class $modify(RSEditorUI, EditorUI) {
    $registerHooks(RotationSliders)

    struct Fields {
        geode::Ref<CCMenuItemSpriteExtra> m_rotationXYControlsButton = nullptr;
        geode::Ref<XYRotationControl> m_rotationXYControls = nullptr;
    };

    void activateRotationXYControl(cocos2d::CCObject* sender);
    void deactivateRotationXYControl();

    void updateButtons();

    cocos2d::CCPoint getSelectionCenter();

    void activateScaleControl(cocos2d::CCObject* sender);
    void activateTransformControl(cocos2d::CCObject* sender);
    void activateRotationControl(cocos2d::CCObject* sender);

    /*void activateTransformControl(CCObject* sender);
    void selectAllWithDirection(bool left);
    void deselectAll();
    void deleteSmartBlocksFromObjects(CCArray* objects);*/
};
