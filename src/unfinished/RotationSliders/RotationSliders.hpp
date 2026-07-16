#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>
#include "XYRotationControl.hpp"

using namespace tinker::ui;

class $editorModule(RotationSliders) {
    void onEditor();
};

class $modify(RSEditorUI, EditorUI) {
    $registerEditorHooks(RotationSliders)

    struct Fields {
        Ref<CCMenuItemSpriteExtra> m_rotationXYControlsButton;
        Ref<XYRotationControl> m_rotationXYControls;
    };

    void activateRotationXYControl(CCObject* sender);
    void deactivateRotationXYControl();

    void updateButtons();

    CCPoint getSelectionCenter();

    void activateScaleControl(CCObject* sender);
    void activateTransformControl(CCObject* sender);
    void activateRotationControl(CCObject* sender);

    /*void activateTransformControl(CCObject* sender);
    void selectAllWithDirection(bool left);
    void deselectAll();
    void deleteSmartBlocksFromObjects(CCArray* objects);*/
};
